#pragma once
#include "Base/WinApp/WinApp.h"
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

class SRV{
public: // -- public メンバ関数 -- // 

	SRV() = default;
	~SRV() = default;

	void Initialize(DirectXCommon* dx);
	void CreateShaderResourceView();
	void CreateSRVDescriptorHeap();
	int LoadTexture(const std::string filePath);
	int SetStructuredBuffer(int32_t kNumInstance, Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource);
	
	int32_t CreateRenderTextureSRV(ID3D12Resource* pResource);

	// メタデータ取得
	const DirectX::TexMetadata& GetMetaData(uint32_t textureId);

	// 空きIndexを取得する関数
	int32_t GetEmptyIndex();

	// Depth用のSRVを生成する
	int32_t CreateDepthSRV(Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource);

private: // -- private メンバ関数 -- // 


	// コピーコンストラクタと演算子オーバーロードの禁止
	SRV(const SRV& obj) = delete;
	SRV& operator=(const SRV& obj) = delete;

public: // -- public メンバ変数 -- // 

	// SRVはディスクリプタを128つ
	D3D12_CPU_DESCRIPTOR_HANDLE srtHandles[128];
	// SRV用ディスクリプタヒープ
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> srvDescriptorHeap = nullptr;

	// テクスチャデータのマップ
	std::unordered_map<int32_t, TextureData> textureData_;
	// 次に使用可能なテクスチャデータの番地
	int32_t textureId_ = 0;
	int32_t defaultTexId_;
	// パーティクルの使用する番地を1000~にする
	int32_t particleId_ = 1000;

	// インスタンシング
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU;

	// -- RenderTexture用 -- //

	Microsoft::WRL::ComPtr<ID3D12Resource> mRenderTextureResource = nullptr;

private: // -- private メンバ変数 -- // 

	// ポインタ
	DirectXCommon* mDxCommon;

};