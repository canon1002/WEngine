#include "DirectXCommon.h"
#include "../resources/Section/Resource.h"
#include "../object/model/Model.h"
#include "../math/Math.h"

// staticメンバ変数で宣言したインスタンスを初期化
DirectXCommon* DirectXCommon::instance = nullptr;

DirectXCommon::DirectXCommon(){}

DirectXCommon::~DirectXCommon(){}

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
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}	
	#endif // _DEBUG

	// 外部のやつ
	win_ = win;
	srv_ = SRV::GetInstance();

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
	InitializePSO();
	InitializePSOP();

	CreateFinalRenderTargets();
	srv_->Initialize(this);
	CreateDepthStencilView();
	InitializeViewPort();
	// 深度バッファ生成
	//CreateDepthBuffer();
	// フェンスの生成
	CreateFence();

	

}

void DirectXCommon::Delete() {

	// 生成順と逆の順番で開放していく

	CloseHandle(fenceEvent);
	//fence->Release();
	//includeHandler->Release();
	//dxcUtils->Release();
	//graphicsPipelineState->Release();
	//rootSignature->Release();
	srv_->Relese();
	//rtvDescriptorHeap->Release();
	//swapChainResources[0]->Release();
	//swapChainResources[1]->Release();
	//swapChain->Release();
	//commandList->Release();
	//commandAllocator->Release();
	//commandQueue->Release();
	//device->Release();
	////useAdapter->Release();
	//dxgiFactory->Release();

#ifdef _DEBUG
	debugController->Release();
#endif // _DEBUG

	// インスタンスを解放
	delete instance;
	// deleteあとにnullptrを代入する
	instance = nullptr;
}

/// DXGIデバイス初期化
void DirectXCommon::InitializeDXGIDevice() {

	// DXGIファクトリーの生成


	// デバッグレイヤーでエラーと警告を受け取る
#ifdef _DEBUG
	Microsoft::WRL::ComPtr <ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif // _DEBUG
	dxgiFactory = nullptr;
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
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
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), win_->hwnd, &swapChainDesc, nullptr, nullptr,
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
	hr = device_->CreateCommandQueue(&commandQueueDesc,
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
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
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
void DirectXCommon::InitializePSO() {

	// RootSignatureを生成する(P.30)

	// パーティクル・モデル・スプライトごとに分ける...?

	
	///////////////////////////////////////////////////////////////
	///	3Dモデル用 ルートシグネチャ
	///////////////////////////////////////////////////////////////

	// RootSignatureを生成する(P.30)
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// 複数枚のSRVを扱えるように一括で設定をする
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;// offset自動計算

	// RootParamenter作成
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う // b0のbと一致
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号とバインド // b0のと一致

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う // b0のbと一致
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号とバインド // b0のと一致

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;	// レジスタ番号1を使う

	descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	// Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイナリフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;	// ありったけのMipMapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);


	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob,
		&errorBlob
	);
	if (FAILED(hr)) {
		WinAPI::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に作成
	hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayoutの設定を行う(P.32)
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	// BlendStateの設定を行う(P.34)
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasterizerStateの設定を行う(P.36)
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをcompileする(P.37)
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = WinAPI::CompileShader(L"Object3D.VS.hlsl",
		L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = WinAPI::CompileShader(L"Object3D.PS.hlsl",
		L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	// PSOを生成する(P.38)
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ 三角
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように色を打ち込むかの設定(気にしなくていい)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//  DepthStencilの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化
	depthStencilDesc.DepthEnable = true;
	// 書き込みを行う
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//  DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 実際に生成
	hr = device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));


}

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
	hr = D3D12SerializeRootSignature(&particleRootSignatureDesc,
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
	pBlendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasterizerStateの設定を行う(P.36)
	D3D12_RASTERIZER_DESC pRasterizerDesc{};
	// 裏面を表示しない
	pRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	pRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// パーティクル用
	Microsoft::WRL::ComPtr<IDxcBlob> particleVSBlob = WinAPI::CompileShader(L"Particle.VS.hlsl",
		L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(particleVSBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> particlePSBlob = WinAPI::CompileShader(L"Particle.PS.hlsl",
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
	// 書き込みを行う
	particleDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
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

/// レンダーターゲット生成
void DirectXCommon::CreateFinalRenderTargets() {

	// RTV用ディスクリプタヒープの生成
	rtvDescriptorHeap = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	// RTVの設定
	rtvDesc;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2Dテクスチャとして書き込む
	// ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	// RTVを2つ作るのでディスクリプタを２つ用意

	// まずは1つ目を作る
	rtvHandles[0] = rtvStartHandle;
	device_->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
	// 2つ目のディスクリプタハンドルを得る(手動で)
	rtvHandles[1].ptr = rtvHandles[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// 2つ目を作る
	device_->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);

}


/// ディープステンシルビューの生成
void DirectXCommon::CreateDepthStencilView() {

	// リソース生成
	depthStencilTextureResource_ = Resource::CreateDeapStencilTextureResource(device_, win_->kClientWidth, win_->kClientHeight);

	// DSV用のヒープでディスクリプタの数は1。DSVはシェーダー内で触るものではないので、ShaderVisibleはfalse
	dsvDescriptorHeap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	// DSVの設定
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	// DSVHeapの先頭にDSVをつくる
	device_->CreateDepthStencilView(depthStencilTextureResource_.Get(),
		&dsvDesc, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());

	dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

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

/// 深度バッファ生成
void DirectXCommon::CreateDepthBuffer() {

}

/// フェンス生成
void DirectXCommon::CreateFence() {

	// 初期値0でFenceを作る
	fence = nullptr;
	fenceValue = 0;
	hr = device_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));
	// FenceのSignalを待つためのイベントを作成する
	fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);

}

/// 描画前処理
void DirectXCommon::DrawBegin() {

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
	commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);

	// 指定した深度で画面全体をクリアにする
	// フレームの最初に最も遠く(1.0)でクリアする
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// 指定した色で画面全体をクリアにする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };	// 青っぽい色 RGBAの順
	commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);

	// 描画用のディスクリプタヒープの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srv_->srvDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(1, descriptorHeaps);

	// コマンドを積み込む
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
	// RootSignatureを設定。PSOに設定しているが、別途設定が必要
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetPipelineState(graphicsPipelineState.Get());

}

void DirectXCommon::DrawPariticleBegin()
{

	// RootSignatureを設定。PSOに設定しているが、別途設定が必要
	commandList->SetGraphicsRootSignature(particleRootSignature.Get());
	commandList->SetPipelineState(pGraphicsPipelineState.Get());

}

/// 描画後処理
void DirectXCommon::DrawEnd() {

	// ImGuiの描画
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

	// 画面に描く処理は全て終わり、画面に映すので状態を遷移
	// 今回はRenderTargetからPresentにする
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	// TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);

	// コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	hr = commandList->Close();
	assert(SUCCEEDED(hr));

	// GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandlists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(1, commandlists);
	// GPUとOSに画面の交換を行うように通知する
	swapChain->Present(1, 0);

	// コマンドの実行完了を待つ
	commandQueue->Signal(fence.Get(), ++fenceValue);

	// FPS固定
	UpdateFixFPS();

	// Fenceの値が指定したsignal値にたどりついているか確認する
	// GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence->GetCompletedValue() != fenceValue)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		// 指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence->SetEventOnCompletion(fenceValue, fenceEvent);
		// イベントを待つ
		WaitForSingleObject(fenceEvent, INFINITE);
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


	// 返す用の変数を宣言
	Microsoft::WRL::ComPtr<ID3D12Resource> result = nullptr;

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

	hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResouceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&result));
	assert(SUCCEEDED(hr));

	return result;
}

/// ディスクリプタヒープの生成
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(
	Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {

	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap>  resultDescriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&resultDescriptorHeap));
	// ディスクリプタヒープが生成できなかったら起動できない
	assert(SUCCEEDED(hr));
	// 生成完了のログをだす
	WinAPI::Log("ディスクリプタヒープの生成に成功\n");
	return resultDescriptorHeap;
}


// FPS固定初期化
void DirectXCommon::InitFixFPS() {
	// 現在時間を記録
	reference_ = std::chrono::steady_clock::now();
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
		std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 1/60秒 (よりわずかに短い時間) 立っていない場合
	if (elapsed < kMinCheckTime) {
		// 1/60	秒経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime)
		{
			// 1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	// 現在時間を記録
	reference_ = std::chrono::steady_clock::now();

}
