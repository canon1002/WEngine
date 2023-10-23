#pragma once
#include "WinAPI.h"
#include <wrl.h>

using namespace Microsoft::WRL;

// 前方宣言
class MatrixCamera;
struct Vector4;
struct Matrix4x4;

class DirectX
{
private:
	
	DirectX();
	~DirectX();

public: // ** 静的メンバ関数 ** //

	// コピーコンストラクタと演算子オーバーロードの禁止
	DirectX(const DirectX& obj) = delete;
	DirectX& operator=(const DirectX& obj) = delete;

	// インスタンスを取得
	static DirectX* GetInstance();



public: // ** メンバ関数 ** //

	/// <summary>
	/// 
	/// </summary>
	/// <param name="win"></param>
	/// <param name="mainCamera"></param>
	void Initialize(WinAPI* win, MatrixCamera* mainCamera);

	/// <summary>
	/// 
	/// </summary>
	void Delete();

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
	/// レンダーターゲット生成
	/// </summary>
	void CreateFinalRenderTargets();

	/// <summary>
	/// 深度バッファ生成
	/// </summary>
	void CreateDepthBuffer();

	/// <summary>
	/// フェンス生成
	/// </summary>
	void CreateFence();

	/// <summary>
	/// 
	/// </summary>
	void InitializeDXC();
	
	/// <summary>
	/// 
	/// </summary>
	void InitializePSO();
	
	/// <summary>
	/// 
	/// </summary>
	void InitializeViewPort();

	/// <summary>
	/// 
	/// </summary>
	void DrawBegin();

	/// <summary>
	/// 
	/// </summary>
	void DrawEnd();

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

public: // ** メンバ変数 ** //
	
	//
	WinAPI* win_ = nullptr;
	MatrixCamera* mainCamera_ = nullptr;

	
	// DXGIファクトリーの生成
	IDXGIFactory7* dxgiFactory = nullptr;
	// D3D12Deviceの生成
	ID3D12Device* device = nullptr;

	HRESULT hr;

	// コマンドアロケータ
	ID3D12CommandAllocator* commandAllocator = nullptr;
	// コマンドキュー
	ID3D12CommandQueue* commandQueue = nullptr;
	// コマンドリスト
	ID3D12GraphicsCommandList* commandList = nullptr;

	// スワップチェインを生成する
	IDXGISwapChain4* swapChain = nullptr;
	// SwapChainからResourceを引っ張ってくる
	ID3D12Resource* swapChainResources[2] = { nullptr };

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	// RTVを2つ作るのでディスクリプタを２つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	
	// ビューポート
	D3D12_VIEWPORT viewport = {};
	// シザー矩形
	D3D12_RECT scissorRect = {};
	// ディスクリプタヒープ
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	// グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState = nullptr;
	// ルートシグネチャー
	ID3D12RootSignature* rootSignature = nullptr;

	// dxCompilerを初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	// 現時点では#includeしないが、includeに対応するための設定を行っておく
	IDxcIncludeHandler* includeHandler = nullptr;

	// フェンス
	ID3D12Fence* fence = nullptr;
	uint64_t fenceValue = 0;
	HANDLE fenceEvent;

};
