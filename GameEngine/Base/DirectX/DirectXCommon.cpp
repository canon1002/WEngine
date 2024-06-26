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
		IID_PPV_ARGS(&mCommandList));
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

void DirectXCommon::SetResourceBarrier(
	ID3D12GraphicsCommandList* commandList,
	ID3D12Resource* resource,
	D3D12_RESOURCE_STATES stateBefore,
	D3D12_RESOURCE_STATES stateAfter) {

	D3D12_RESOURCE_BARRIER barrier = {};
	
	// Transition Barrier
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	
	// Flag
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	
	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = resource;
	
	// 遷移前(現在)のResourceState
	barrier.Transition.StateBefore = stateBefore;
	
	// 遷移後のResourceState
	barrier.Transition.StateAfter = stateAfter;
	
	//
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	
	// リソースバリアを張る
	commandList->ResourceBarrier(1, &barrier);
};



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
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

	// バリアの設定
	SetResourceBarrier(mCommandList.Get(), swapChainResources[backBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// 描画対象の設定
	SetRenderTargets(mCommandList.Get(), 1, &rtv_->rtvHandles[backBufferIndex], dsv_->mDsvHandle);

	// クリア処理
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f }; // 青っぽい色 RGBAの順
	ClearRenderTargetView(mCommandList.Get(), rtv_->rtvHandles[backBufferIndex], clearColor);
	// ClearDepthStencilView(mCommandList.Get(), dsv_->mDsvHandle, 1.0f, 0); // 必要に応じて

	// ディスクリプタヒープの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srv_->srvDescriptorHeap.Get() };
	SetDescriptorHeaps(mCommandList.Get(), 1, descriptorHeaps);

	// コマンドを積み込む
	mCommandList->RSSetViewports(1, &viewport);
	mCommandList->RSSetScissorRects(1, &scissorRect);
}

void DirectXCommon::PreDrawForRenderTarget() {
	// バリアの設定
	SetResourceBarrier(mCommandList.Get(), rtv_->mRenderTextureResource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// 描画対象の設定
	SetRenderTargets(mCommandList.Get(), 1, &rtv_->rtvHandles[2], dsv_->mDsvHandle);

	// クリア処理
	ClearRenderTargetView(mCommandList.Get(), rtv_->rtvHandles[2], rtv_->kRenderTargetColor);
	ClearDepthStencilView(mCommandList.Get(), dsv_->mDsvHandle, 1.0f, 0);

	// ディスクリプタヒープの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srv_->srvDescriptorHeap.Get() };
	SetDescriptorHeaps(mCommandList.Get(), 1, descriptorHeaps);

	// コマンドを積み込む
	mCommandList->RSSetViewports(1, &viewport);
	mCommandList->RSSetScissorRects(1, &scissorRect);
}

void DirectXCommon::PostDrawForRenderTarget() {
	// バリアの設定
	SetResourceBarrier(mCommandList.Get(), rtv_->mRenderTextureResource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

}

void DirectXCommon::PostDraw() {
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

	// バリアの設定
	SetResourceBarrier(mCommandList.Get(), swapChainResources[backBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	HRESULT hr = mCommandList->Close();
	assert(SUCCEEDED(hr));

	// GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandlists[] = { mCommandList.Get() };
	commandQueue->ExecuteCommandLists(1, commandlists);

	// GPUとOSに画面の交換を行うように通知する
	swapChain->Present(1, 0);

	// コマンドの実行完了を待つ
	commandQueue->Signal(mFence.Get(), ++mFenceValue);

	// FPS固定
	UpdateFixFPS();

	// Fenceの値が指定したsignal値にたどりついているか確認する
	if (mFence->GetCompletedValue() != mFenceValue) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		mFence->SetEventOnCompletion(mFenceValue, mFenceEvent);
		WaitForSingleObject(mFenceEvent, INFINITE);
		CloseHandle(event);
	}

	// 次のフレーム用のコマンドリストを準備
	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = mCommandList->Reset(commandAllocator.Get(), nullptr);
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
