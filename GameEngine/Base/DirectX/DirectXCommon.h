#pragma once
#include "GameEngine/Base/WinApp/WinAPI.h"
#include "RTV.h"
#include "SRV.h"
#include "DSV.h"

#include "Externals/DirectXTex/DirectXTex.h"
#include <chrono>
#include <thread>

// 前方宣言
struct Vector4;
struct ModelData;


// リソースリークチェック
struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker(){
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			debug->Release();
		}
	}
};


class DirectXCommon
{
public:
	
	// 外部からの呼び出し禁止
	DirectXCommon() = default;
	~DirectXCommon() = default;

	// シングルトン インスタンス取得
	static DirectXCommon* GetInstance();


	// FPS固定初期化
	void InitFixFPS();
	// FPS固定更新
	void UpdateFixFPS();

private: // ** 静的メンバ関数 ** //

	// コピーコンストラクタと演算子オーバーロードの禁止
	DirectXCommon(const DirectXCommon& obj) = delete;
	DirectXCommon& operator=(const DirectXCommon& obj) = delete;

public: // ** メンバ関数 ** //

	/// 初期化
	void Initialize(WinAPI* win);

	/// 終了処理
	void Finalize();

	/// <summary>
	/// DXGIデバイス初期化
	/// </summary>
	void InitializeDXGIDevice();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// コマンド関連初期化
	/// </summary>
	void InitializeCommand();

	/// <summary>
	/// フェンス生成
	/// </summary>
	void CreateFence();

	void InitializeDXC();
	void InitializeViewPort();
	
	void PreDrawForRenderTarget();
	void PostDrawForRenderTarget();
	void PreDraw();
	void PostDraw();



	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap,
		uint32_t descriptorSize,uint32_t index){
		D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		handleCPU.ptr += (descriptorSize * index);
		return handleCPU;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap,
		uint32_t descriptorSize,uint32_t index){
		D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		handleGPU.ptr += (descriptorSize * index);
		return handleGPU;
	}

	/// バッファリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	/// ディスクリプタヒープの生成
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> CreateDescriptorHeap(
		Microsoft::WRL::ComPtr <ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

public: // ** メンバ変数 ** //
	
	// 外部
	WinAPI* win_ = nullptr;
	std::unique_ptr<RTV> rtv_ = nullptr;
	std::unique_ptr<SRV> srv_ = nullptr;
	std::unique_ptr<DSV> dsv_ = nullptr;

	// DXGIファクトリーの生成
	Microsoft::WRL::ComPtr <IDXGIFactory7> dxgiFactory = nullptr;
	// D3D12Deviceの生成
	Microsoft::WRL::ComPtr <ID3D12Device> device_ = nullptr;
	// コマンドアロケータ
	Microsoft::WRL::ComPtr < ID3D12CommandAllocator> commandAllocator = nullptr;
	// コマンドキュー
	Microsoft::WRL::ComPtr < ID3D12CommandQueue> commandQueue = nullptr;
	// コマンドリスト
	Microsoft::WRL::ComPtr < ID3D12GraphicsCommandList> commandList = nullptr;
	// スワップチェイン
	Microsoft::WRL::ComPtr < IDXGISwapChain4> swapChain = nullptr;
	// スワップチェーンデスク
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	// SwapChainResource
	Microsoft::WRL::ComPtr < ID3D12Resource> swapChainResources[2] = { nullptr };

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};

	// ビューポート
	D3D12_VIEWPORT viewport = {};
	// シザー矩形
	D3D12_RECT scissorRect = {};

	// 返す用の変数を宣言
	Microsoft::WRL::ComPtr<ID3D12Resource> result_ = nullptr;


	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> pGraphicsPipelineState = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr < ID3D12RootSignature> particleRootSignature = nullptr;

	// dxCompilerを初期化
	Microsoft::WRL::ComPtr < IDxcUtils> dxcUtils = nullptr;
	Microsoft::WRL::ComPtr < IDxcCompiler3> dxcCompiler = nullptr;
	// 現時点では#includeしないが、includeに対応するための設定を行っておく
	Microsoft::WRL::ComPtr < IDxcIncludeHandler> includeHandler = nullptr;

	// フェンス
	Microsoft::WRL::ComPtr < ID3D12Fence> mFence = nullptr;
	uint64_t mFenceValue = 0;
	HANDLE mFenceEvent;

	// 記録時間(FPS固定用)
	std::chrono::steady_clock::time_point mReference;

private:

	// インスタンス
	static DirectXCommon* instance;

#ifdef _DEBUG

	
	//ID3D12Debug1* mDebugController;

#endif // _DEBUG


};
