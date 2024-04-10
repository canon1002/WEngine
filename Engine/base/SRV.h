#pragma once
#include "WinAPI.h"
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

class SRV
{
private:

	SRV() = default;
	~SRV() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	SRV(const SRV& obj) = delete;
	SRV& operator=(const SRV& obj) = delete;

public: // ** 静的メンバ関数 ** //

	// インスタンスを取得
	static SRV* GetInstance();
	// 開放する
	void Finalize();

public:

	void Initialize(DirectXCommon* dx);
	void CreateShaderResourceView();
	void CreateSRVDescriptorHeap();
	int LoadTexture(const std::string filePath);
	int SetStructuredBuffer(
		int32_t kNumInstance, Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource
	);
	
	// メタデータ取得
	const DirectX::TexMetadata& GetMetaData(uint32_t textureId);

private:

	// インスタンス
	static SRV* instance;

	// ポインタ
	DirectXCommon* dx_;

public:

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

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU;

};