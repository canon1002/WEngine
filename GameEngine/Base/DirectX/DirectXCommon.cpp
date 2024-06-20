#include "DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Model/Model.h"
#include "GameEngine/Math/Math.h"

// staticメンバ変数で宣言したインスタンスを初期化
DirectXCommon* DirectXCommon::instance = nullptr;

// インスタンスを取得
DirectXCommon* DirectXCommon::GetInstance() {
	// 関数内staticは初めて通ったときのみ実行される
	//static DirectXCommon* instance = nullptr;
	if (instance == nullptr) {
		instance = new DirectXCommon;
	}
	return instance;
}

void DirectXCommon::Initialize(WinAPI* win) {

	// FPS固定
	InitFixFPS();

	// デバッグレイヤーでエラーと警告を受け取る
	#ifdef _DEBUG
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&mDebugController)))) {
		// デバッグレイヤーを有効化する
		mDebugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行うようにする
		mDebugController->SetEnableGPUBasedValidation(TRUE);
	}	
	#endif // _DEBUG

	// 外部のやつ
	win_ = win;
	rtv_ = std::make_unique<RTV>();
	srv_ = std::make_unique<SRV>();
	dsv_ = std::make_unique<DSV>();
	// デバイスの初期化
	InitializeDXGIDevice();

	// 警告やエラーが発生した際に停止させる
	#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			// Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			// https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したメッセージを抑制する
		infoQueue->PushStorageFilter(&filter);
		// 解放
		infoQueue->Release();

	}
	#endif // _DEBUG

	// コマンドの初期化
	InitializeCommand();
	// スワップチェーンの生成
	CreateSwapChain();
	// レンダーターゲットビュー
	// dxCompiler
	InitializeDXC();
	// pipelineStateObject
	InitializePSOP();

	// レンダーターゲットを生成
	rtv_->Initialize(this);
	// シェーダーリソースを生成
	srv_->Initialize(this);
	// ディープステンシルの生成
	dsv_->Initialize(this);

	InitializeViewPort();
	// 深度バッファ生成
	//CreateDepthBuffer();
	// フェンスの生成
	CreateFence();
}

void DirectXCommon::Finalize() {
	CloseHandle(mFenceEvent);
}

/// DXGIデバイス初期化
void DirectXCommon::InitializeDXGIDevice() {

	// DXGIファクトリーの生成
	dxgiFactory = nullptr;
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(hr));

	// 使用するアダプタ用の変数。最初にnullptrを入れておく
	IDXGIAdapter4* useAdapter = nullptr;
	// HRESULTはWindows系のエラーコードであり、
	// 関数が成功したかどうかをSUCCEEDEDマクロで判断できる
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	// 初期化の根本的な部分でエラーが起きた場合はプログラムが間違っているかどうか、
	// どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));

	// アダプターの情報を取得
	DXGI_ADAPTER_DESC3 adapterDesc{};
	// 良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; i++) {

		// アダプターの情報を取得
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));// 取得できないのは一大事
		// ソフトウェアアダプタでなければ採用する
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// 採用したアダプタ情報をログに出力。wstringの方なので注意
			WinAPI::Log(WinAPI::ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;
	}
	// 適切なアダプタが見つからなかった場合は起動できない
	assert(useAdapter != nullptr);

	// 機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1" ,"12.0" };
	// 高い順の生成できるか試す
	for (size_t i = 0; i < _countof(featureLevels); i++) {
		//採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device_));
		// 指定したレベルでデバイスが生成できたか確認
		if (SUCCEEDED(hr)) {
			// 生成できたのでログ出力を行ってループを抜ける
			WinAPI::Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	// デバイス生成がうまく行かなかったら起動できない
	assert(device_ != nullptr);
	WinAPI::Log("Complete create D3D12Device!!!\n");// 初期化完了のログをだす

}

/// スワップチェーンの生成
void DirectXCommon::CreateSwapChain() {

	swapChainDesc.Width = win_->kClientWidth;	// 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Height = win_->kClientHeight; // 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 色の形式
	swapChainDesc.SampleDesc.Count = 1;	// マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描画のターゲットとして利用する
	swapChainDesc.BufferCount = 2;	// ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// モニタに映したら、中身を破棄
	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), win_->hwnd, &swapChainDesc, nullptr, nullptr,
		reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));
	assert(SUCCEEDED(hr));
	WinAPI::Log("Complete create SwapChain!!!\n");// 生成完了のログをだす


	// それぞれ生成し、できなければ起動できない
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	assert(SUCCEEDED(hr));
	WinAPI::Log("Complete create swapChainResources[0]!!!\n");// 生成完了のログをだす
	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr));
	WinAPI::Log("Complete create swapChainResources[1]!!!\n");// 生成完了のログをだす

}

/// コマンド関連初期化
void DirectXCommon::InitializeCommand() {

	// コマンドキューを生成する
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	HRESULT hr = device_->CreateCommandQueue(&commandQueueDesc,
		IID_PPV_ARGS(&commandQueue));
	// コマンドキューの生成がうまく行かなかったら起動できない
	assert(SUCCEEDED(hr));
	WinAPI::Log("Complete create CommandQueue!!!\n");// 生成完了のログをだす

	// コマンドアロケータを生成する
	hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	// コマンドアロケータの生成がうまく行かなかったら起動できない
	assert(SUCCEEDED(hr));
	WinAPI::Log("Complete create CommandAllocator!!!\n");// 生成完了のログをだす

	// コマンドリスト生成する
	hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr,
		IID_PPV_ARGS(&commandList));
	// コマンドリストの生成がうまく行かなかったら起動できない
	assert(SUCCEEDED(hr));
	WinAPI::Log("Complete create CommandList!!!\n");// 生成完了のログをだす

}

/// dxCompilerの初期化
void DirectXCommon::InitializeDXC() {

	dxcUtils = nullptr;
	dxcCompiler = nullptr;
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	WinAPI::Log("Complete create dxcUtils!!!\n");// 生成完了のログをだす
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));
	WinAPI::Log("Complete create dxcCompiler!!!\n");// 生成完了のログをだす

	// 現時点では#includeしないが、includeに対応するための設定を行っておく
	includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));
	WinAPI::Log("Complete create includeHandler!!!\n");// 生成完了のログをだす

};

/// PipelineStateObjectの初期化
void DirectXCommon::InitializePSOP() {

	// RootSignatureを生成する(P.30)
	
	///////////////////////////////////////////////////////////////
	///	パーティクル用 ルートシグネチャ
	///////////////////////////////////////////////////////////////

		// 宣言する
	D3D12_ROOT_SIGNATURE_DESC particleRootSignatureDesc{};
	particleRootSignatureDesc.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootSignatureの変更 -- パーティクル
	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;
	descriptorRangeForInstancing[0].NumDescriptors = 1;
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;// offset自動計算

	// RootParamenter作成
	D3D12_ROOT_PARAMETER paticleRootParameters[3] = {};

	paticleRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う // b0のbと一致
	paticleRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	paticleRootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号とバインド // b0のと一致

	paticleRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	paticleRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
	paticleRootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing; // Tableの中身の配列を指定
	paticleRootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing); // Tableの中身の配列を指定

	paticleRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	paticleRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	paticleRootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
	paticleRootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);

	particleRootSignatureDesc.pParameters = paticleRootParameters; // ルートパラメータ配列へのポインタ
	particleRootSignatureDesc.NumParameters = _countof(paticleRootParameters); // 配列の長さ

	// Samplerの設定
	D3D12_STATIC_SAMPLER_DESC pStaticSamplers[1] = {};
	pStaticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイナリフィルタ
	pStaticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	pStaticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pStaticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pStaticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	pStaticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;	// ありったけのMipMapを使う
	pStaticSamplers[0].ShaderRegister = 0;// レジスタ番号0を使う
	pStaticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	particleRootSignatureDesc.pStaticSamplers = pStaticSamplers;
	particleRootSignatureDesc.NumStaticSamplers = _countof(pStaticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> particleSignatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> particleErrorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&particleRootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&particleSignatureBlob,
		&particleErrorBlob
	);
	if (FAILED(hr)) {
		WinAPI::Log(reinterpret_cast<char*>(particleErrorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に作成
	hr = device_->CreateRootSignature(0, particleSignatureBlob->GetBufferPointer(),
		particleSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&particleRootSignature));
	assert(SUCCEEDED(hr));

	// InputLayoutの設定を行う(P.32)
	D3D12_INPUT_ELEMENT_DESC pInputElementDescs[3] = {};
	pInputElementDescs[0].SemanticName = "POSITION";
	pInputElementDescs[0].SemanticIndex = 0;
	pInputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	pInputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	pInputElementDescs[1].SemanticName = "TEXCOORD";
	pInputElementDescs[1].SemanticIndex = 0;
	pInputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	pInputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	pInputElementDescs[2].SemanticName = "COLOR";
	pInputElementDescs[2].SemanticIndex = 0;
	pInputElementDescs[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	pInputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC pInputLayoutDesc{};
	pInputLayoutDesc.pInputElementDescs = pInputElementDescs;
	pInputLayoutDesc.NumElements = _countof(pInputElementDescs);

	// BlendStateの設定を行う(P.34)
	D3D12_BLEND_DESC pBlendDesc{};
	// すべての色要素を書き込む
	pBlendDesc.RenderTarget[0].RenderTargetWriteMask =D3D12_COLOR_WRITE_ENABLE_ALL;
	pBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	pBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	pBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	pBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	pBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	pBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	pBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	pBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	// RasterizerStateの設定を行う(P.36)
	D3D12_RASTERIZER_DESC pRasterizerDesc{};
	// 裏面を表示しない
	pRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	pRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// パーティクル用
	Microsoft::WRL::ComPtr<IDxcBlob> particleVSBlob = WinAPI::CompileShader(L"Shaders/Particle.VS.hlsl",
		L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(particleVSBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> particlePSBlob = WinAPI::CompileShader(L"Shaders/Particle.PS.hlsl",
		L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(particlePSBlob != nullptr);

	// PSOを生成する(P.38)
	D3D12_GRAPHICS_PIPELINE_STATE_DESC particleGraphicsPipelineStateDesc{};
	particleGraphicsPipelineStateDesc.pRootSignature = particleRootSignature.Get();
	particleGraphicsPipelineStateDesc.InputLayout = pInputLayoutDesc;
	particleGraphicsPipelineStateDesc.VS = { particleVSBlob->GetBufferPointer(),
		particleVSBlob->GetBufferSize() };
	particleGraphicsPipelineStateDesc.PS = { particlePSBlob->GetBufferPointer(),
		particlePSBlob->GetBufferSize() };
	particleGraphicsPipelineStateDesc.BlendState = pBlendDesc;
	particleGraphicsPipelineStateDesc.RasterizerState = pRasterizerDesc;
	// 書き込むRTVの情報
	particleGraphicsPipelineStateDesc.NumRenderTargets = 1;
	particleGraphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ 三角
	particleGraphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように色を打ち込むかの設定(気にしなくていい)
	particleGraphicsPipelineStateDesc.SampleDesc.Count = 1;
	particleGraphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//  DepthStencilの設定を行う
	D3D12_DEPTH_STENCIL_DESC particleDepthStencilDesc{};
	// Depthの機能を有効化
	particleDepthStencilDesc.DepthEnable = true;
	// 書き込みを行わないようにする
	particleDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// 比較関数はLessEqual。つまり、近ければ描画される
	particleDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//  DepthStencilの設定
	particleGraphicsPipelineStateDesc.DepthStencilState = particleDepthStencilDesc;
	particleGraphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 実際に生成
	hr = device_->CreateGraphicsPipelineState(&particleGraphicsPipelineStateDesc,
		IID_PPV_ARGS(&pGraphicsPipelineState));
	assert(SUCCEEDED(hr));


}


///
void DirectXCommon::InitializeViewPort() {

	// ビューポート
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = (float)win_->kClientWidth;
	viewport.Height = (float)win_->kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// シザー矩形
	// 基本的にビューポートと同じ矩形が描画されるようにする
	scissorRect.left = 0;
	scissorRect.right = win_->kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = win_->kClientHeight;
}

/// フェンス生成
void DirectXCommon::CreateFence() {

	// 初期値0でFenceを作る
	mFenceValue = 0;
	HRESULT hr = device_->CreateFence(mFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
	assert(SUCCEEDED(hr));
	// FenceのSignalを待つためのイベントを作成する
	mFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(mFenceEvent != nullptr);

}

/// 描画前処理
void DirectXCommon::PreDraw() {

	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

	// TransitionBarrierの設定
	// 今回のバリアはトランジション
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();
	// 遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	// 遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);


	// 描画先のRTVとDSVを設定する
	commandList->OMSetRenderTargets(1, &rtv_->rtvHandles[backBufferIndex], false, &dsv_->mDsvHandle);

	// 指定した深度で画面全体をクリアにする
	// フレームの最初に最も遠く(1.0)でクリアする
	//commandList->ClearDepthStencilView(dsv_->mDsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// 指定した色で画面全体をクリアにする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };	// 青っぽい色 RGBAの順
	commandList->ClearRenderTargetView(rtv_->rtvHandles[backBufferIndex], clearColor, 0, nullptr);

	// 描画用のディスクリプタヒープの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srv_->srvDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(1, descriptorHeaps);

	// コマンドを積み込む
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
}

void DirectXCommon::PreDrawForRenderTarget(){

	// これから書き込むバックバッファのインデックスを取得
	//UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

	// TransitionBarrierの設定
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = rtv_->mRenderTextureResource.Get();
	// 遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	// 遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);

	// 描画先のRTVとDSVを設定する
	commandList->OMSetRenderTargets(1, &rtv_->rtvHandles[2],
		false, &dsv_->mDsvHandle);

	// 指定した色で画面全体をクリアにする
	commandList->ClearRenderTargetView(rtv_->rtvHandles[2],
		rtv_->kRenderTargetColor, 0, nullptr);

	// 指定した深度で画面全体をクリアにする
	// フレームの最初に最も遠く(1.0)でクリアする
	commandList->ClearDepthStencilView(dsv_->mDsvHandle,
		D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// 描画用のディスクリプタヒープの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srv_->srvDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(1, descriptorHeaps);

	// コマンドを積み込む
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);

}

void DirectXCommon::PostDrawForRenderTarget(){

	// TransitionBarrierの設定
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = rtv_->mRenderTextureResource.Get();
	// 遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// 遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	// TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);

}

void DirectXCommon::DrawPariticleBegin()
{

	// RootSignatureを設定。PSOに設定しているが、別途設定が必要
	commandList->SetGraphicsRootSignature(particleRootSignature.Get());
	commandList->SetPipelineState(pGraphicsPipelineState.Get());

}

/// 描画後処理
void DirectXCommon::PostDraw() {

	// 画面に描く処理は全て終わり、画面に映すので状態を遷移
	// 今回はRenderTargetからPresentにする
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	// TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);

	// コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	HRESULT hr = commandList->Close();
	assert(SUCCEEDED(hr));

	// GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandlists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(1, commandlists);
	// GPUとOSに画面の交換を行うように通知する
	swapChain->Present(1, 0);

	// コマンドの実行完了を待つ
	commandQueue->Signal(mFence.Get(), ++mFenceValue);

	// FPS固定
	UpdateFixFPS();

	// Fenceの値が指定したsignal値にたどりついているか確認する
	// GetCompletedValueの初期値はFence作成時に渡した初期値
	if (mFence->GetCompletedValue() != mFenceValue)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		// 指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		mFence->SetEventOnCompletion(mFenceValue, mFenceEvent);
		// イベントを待つ
		WaitForSingleObject(mFenceEvent, INFINITE);
		CloseHandle(event);
	}

	// 次のフレーム用のコマンドリストを準備
	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator.Get(), nullptr);
	assert(SUCCEEDED(hr));

}

/// バッファリソースの生成
Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {

	result_ = nullptr;
	// VertexResourceを生成する(P.42)
	// 頂点リソース用のヒープ設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;// UploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResouceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResouceDesc.Width = sizeInBytes;// リソースのサイズ。
	// バッファの場合はこれらは1にする決まり
	vertexResouceDesc.Height = 1;
	vertexResouceDesc.DepthOrArraySize = 1;
	vertexResouceDesc.MipLevels = 1;
	vertexResouceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	vertexResouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT hr;
	hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResouceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&result_));
	assert(SUCCEEDED(hr));

	return result_;
}

/// ディスクリプタヒープの生成
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(
	Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {

	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap>  resultDescriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr;
	hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&resultDescriptorHeap));
	// ディスクリプタヒープが生成できなかったら起動できない
	assert(SUCCEEDED(hr));
	// 生成完了のログをだす
	WinAPI::Log("ディスクリプタヒープの生成に成功\n");
	return resultDescriptorHeap;
}


// FPS固定初期化
void DirectXCommon::InitFixFPS() {
	// 現在時間を記録
	mReference = std::chrono::steady_clock::now();
}

// FPS固定更新
void DirectXCommon::UpdateFixFPS() {
	// 1/60秒ピッタリの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
	// 1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	// 現在時間を取得
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	// 前回記録からの経過時間を取得
	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(now - mReference);

	// 1/60秒 (よりわずかに短い時間) 立っていない場合
	if (elapsed < kMinCheckTime) {
		// 1/60	秒経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now() - mReference < kMinTime)
		{
			// 1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	// 現在時間を記録
	mReference = std::chrono::steady_clock::now();

}
