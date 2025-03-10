#pragma once
#include "GameEngine/Base/WinApp/WinAPI.h"
#include <unordered_map>
#include "Externals/DirectXTex/DirectXTex.h"

class DirectXCommon;

struct TextureData {
	std::string filePath;
	DirectX::TexMetadata metadata;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediaResource = nullptr;
};

class SRV {

public: // -- 公開 メンバ関数 -- // 

	// コンストラクタ
	SRV() = default;
	// デストラクタ
	~SRV() = default;
	// 初期化
	void Init();

	// 確保
	uint32_t Allocate();

	// SRV生成(テクスチャ用)
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT format, UINT mipLevels);
	// SRV生成(CubeMap用)
	void CreateSRVforTextureCubeMap(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT format);
	// SRV生成(Structured Buffer用)
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

	// SRVセットコマンド
	void SetGraphicsRootDescriptorTable(UINT rootParameterIndex, uint32_t srvIndex);

	// 描画前処理
	void PreDraw();


	
	int32_t CreateRenderTextureSRV(ID3D12Resource* pResource);

	// メタデータ取得
	const DirectX::TexMetadata& GetMetaData(uint32_t textureId);

	// 空きIndexを取得する関数
	int32_t GetEmptyIndex();

	// Depth用のSRVを生成する
	int32_t CreateDepthSRV(Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource);

	inline D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index) {
		D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		handleCPU.ptr += (mDescriptorSize * index);
		return handleCPU;
	}

	inline D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index) {
		D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		handleGPU.ptr += (mDescriptorSize * index);
		return handleGPU;
	}

private: // -- private メンバ関数 -- // 

	// コピーコンストラクタと演算子オーバーロードの禁止
	SRV(const SRV& obj) = delete;
	SRV& operator=(const SRV& obj) = delete;


public: // -- public メンバ変数 -- // 

	// SRVはディスクリプタを128つ
	//D3D12_CPU_DESCRIPTOR_HANDLE srtHandles[128];
	
	// 最大SRVサイズ
	static const uint32_t kMaxSRVCount = 128;
	// SRV用デスクリプタのサイズ
	uint32_t mDescriptorSize;
	// SRV用デスクリプタヒープ
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> mDescriptorHeap = nullptr;

	// テクスチャデータのマップ
	std::unordered_map<int32_t, TextureData> mTextureData;
	// 次に使用可能な番地
	uint32_t mUseIndex = 0;


	// -- RenderTexture用 -- //

	Microsoft::WRL::ComPtr<ID3D12Resource> mRenderTextureResource = nullptr;

private: // -- private メンバ変数 -- // 


};