#pragma once
#include "WinAPI.h"
#include "../../../Externals/DirectXTex/DirectXTex.h"

// 前方宣言
class MatrixCamera;
struct Vector4;
struct Matrix4x4;

class DirectXCommon
{
private:
	
	DirectXCommon();
	~DirectXCommon();

public: // ** 静的メンバ関数 ** //

	// コピーコンストラクタと演算子オーバーロードの禁止
	DirectXCommon(const DirectXCommon& obj) = delete;
	DirectXCommon& operator=(const DirectXCommon& obj) = delete;

	// インスタンスを取得
	static DirectXCommon* GetInstance();

	// 開放する
	static void Relese();


public: // ** メンバ関数 ** //

	/// <summary>
	/// 
	/// </summary>
	/// <param name="win"></param>
	void Initialize(WinAPI* win);

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
	/// シェーダーリソースビュー生成
	/// </summary>
	void CreateShaderResourceView();

	/// <summary>
	/// ディープステンシルビューの生成
	/// </summary>
	void CreateDepthStencilView();

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

	DirectX::ScratchImage LoadTexture(const std::string& filePath);

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
	
	//
	WinAPI* win_ = nullptr;
	
	// DXGIファクトリーの生成
	Microsoft::WRL::ComPtr <IDXGIFactory7> dxgiFactory = nullptr;
	// D3D12Deviceの生成
	Microsoft::WRL::ComPtr <ID3D12Device> device = nullptr;
	//
	HRESULT hr;

	// コマンドアロケータ
	Microsoft::WRL::ComPtr < ID3D12CommandAllocator> commandAllocator = nullptr;
	// コマンドキュー
	Microsoft::WRL::ComPtr < ID3D12CommandQueue> commandQueue = nullptr;
	// コマンドリスト
	Microsoft::WRL::ComPtr < ID3D12GraphicsCommandList> commandList = nullptr;

	// スワップチェインを生成する
	Microsoft::WRL::ComPtr < IDXGISwapChain4> swapChain = nullptr;
	// スワップチェーンデスク
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	// SwapChainからResourceを引っ張ってくる
	Microsoft::WRL::ComPtr < ID3D12Resource> swapChainResources[2] = { nullptr };

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};

	// ビューポート
	D3D12_VIEWPORT viewport = {};
	// シザー矩形
	D3D12_RECT scissorRect = {};

	// RTVを2つ作るのでディスクリプタを２つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	// RTV用ディスクリプタヒープ
	Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> rtvDescriptorHeap = nullptr;

	// SRVはディスクリプタを128つ
	D3D12_CPU_DESCRIPTOR_HANDLE srtHandles[128];
	// SRV用ディスクリプタヒープ
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> srvDescriptorHeap = nullptr;
	//SRVを制作するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2_;
	// テクスチャリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2_ = nullptr;
	// リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediaResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediaResource2 = nullptr;


	// DSV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;
	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	//　ディープステンシル用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilTextureResource_ = nullptr;
	//　ディスクリプタ
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;

	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature = nullptr;

	// dxCompilerを初期化
	Microsoft::WRL::ComPtr < IDxcUtils> dxcUtils = nullptr;
	Microsoft::WRL::ComPtr < IDxcCompiler3> dxcCompiler = nullptr;
	// 現時点では#includeしないが、includeに対応するための設定を行っておく
	Microsoft::WRL::ComPtr < IDxcIncludeHandler> includeHandler = nullptr;

	// フェンス
	Microsoft::WRL::ComPtr < ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;
	HANDLE fenceEvent;

	
private:

	// インスタンス
	static DirectXCommon* instance;

};
