#pragma once
#include "GameEngine/Base/WinApp/WinAPI.h"
#include "RTV.h"
#include "SRV.h"
#include "DSV.h"

#include "Externals/DirectXTex/DirectXTex.h"
#include <chrono>
#include <thread>

// シャドウで使う
#include <DirectXMath.h>

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

/// <summary>
/// DirectX関連クラス
/// </summary>
class DirectXCommon
{
public: // -- 公開 メンバ関数 -- //
	
	// シングルトン インスタンス取得
	static DirectXCommon* GetInstance();
	// FPS固定初期化
	void InitFixFPS();
	// FPS固定更新
	void UpdateFixFPS();

	/// 初期化
	void Init();

	/// 終了処理
	void Final();

	// -- 描画関係処理 -- //

	// RenderTargetへの描画前処理
	void PreDrawForRenderTarget();
	// RenderTargetへの描画後処理
	void PostDrawForRenderTarget();
	// Swapchainへの描画前処理	
	void PreDraw();
	// Swapchainへの描画後処理
	void PostDraw();

	/// バッファリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	/// ディスクリプタヒープの生成
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> CreateDescriptorHeap(
		Microsoft::WRL::ComPtr <ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

private: // -- 非公開 メンバ関数 -- //

	// コンストラクタ
	DirectXCommon() = default;
	// デストラクタ
	~DirectXCommon() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	DirectXCommon(const DirectXCommon& obj) = delete;
	DirectXCommon& operator=(const DirectXCommon& obj) = delete;


	// -- 生成・初期化関数 -- //

	// DXGIデバイス初期化
	void InitDXGIDevice();

	// スワップチェーンの生成
	void CreateSwapChain(); 

	// コマンド関連初期化
	void InitCommand(); 

	// フェンス生成
	void CreateFence();

	void InitializeDXC();

	// ビューポートの初期化
	void InitViewPort();

	// リソースバリアの変更
	void SetResourceBarrier(
		ID3D12GraphicsCommandList* commandList,ID3D12Resource* resource,
		D3D12_RESOURCE_STATES stateBefore,D3D12_RESOURCE_STATES stateAfter);

	// RTVのクリア処理
	void ClearRenderTargetView(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, const float clearColor[4]) {
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	}

	// DSVのクリア処理
	void ClearDepthStencilView(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, float depth, UINT8 stencil) {
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, depth, stencil, 0, nullptr);
	}

	// RTVのセット
	void SetRenderTargets(ID3D12GraphicsCommandList* commandList, UINT numRenderTargets, const D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandles, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) {
		commandList->OMSetRenderTargets(numRenderTargets, rtvHandles, FALSE, &dsvHandle);
	}

public: // -- 公開 メンバ変数 -- //
	
	std::unique_ptr<RTV> mRtv = nullptr;
	std::unique_ptr<SRV> mSrv = nullptr;
	std::unique_ptr<DSV> mDsv = nullptr;

	// DXGIファクトリーの生成
	Microsoft::WRL::ComPtr<IDXGIFactory7> mDXGIFactory = nullptr;
	// D3D12Deviceの生成
	Microsoft::WRL::ComPtr<ID3D12Device> mDevice = nullptr;
	// コマンドアロケータ
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator = nullptr;
	// コマンドキュー
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue = nullptr;
	// コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList = nullptr;
	// スワップチェイン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain = nullptr;
	// スワップチェーンデスク
	DXGI_SWAP_CHAIN_DESC1 mSwapChainDesc{};
	// SwapChainResource
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainResources[2] = { nullptr };

	// TransitionBarrier
	D3D12_RESOURCE_BARRIER mBarrier{};
	// 現在のBarrierState
	D3D12_RESOURCE_STATES mCurrentBarrierState;

	// ビューポート
	D3D12_VIEWPORT mViewport = {};
	// シザー矩形
	D3D12_RECT mScissorRect = {};

	// 結果の格納先
	Microsoft::WRL::ComPtr<ID3D12Resource> mResult = nullptr;

	// dxCompiler初期化関係
	Microsoft::WRL::ComPtr < IDxcUtils> mDxcUtils = nullptr;
	Microsoft::WRL::ComPtr < IDxcCompiler3> mDxcCompiler = nullptr;

	// includeに対応するための設定
	Microsoft::WRL::ComPtr < IDxcIncludeHandler> mIncludeHandler = nullptr;

	// フェンス
	Microsoft::WRL::ComPtr < ID3D12Fence> mFence = nullptr;
	uint64_t mFenceValue = 0;
	HANDLE mFenceEvent;

	// 記録時間(FPS固定用)
	std::chrono::steady_clock::time_point mReference;

private: // -- 非公開 メンバ変数 -- //

	// インスタンス
	static DirectXCommon* instance;

#ifdef _DEBUG

	// デバッグレイヤー
	//ID3D12Debug1* mDebugController;
	ID3D12Debug1* mDebugController;

#endif // _DEBUG


};
