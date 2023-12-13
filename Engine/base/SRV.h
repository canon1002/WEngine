#pragma once
#include "WinAPI.h"
#include <unordered_map>
#include "../../../Externals/DirectXTex/DirectXTex.h"

class DirectXCommon;

struct TextureData {
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediaResource = nullptr;
};

class SRV
{
private:

	SRV();
	~SRV();

public: // ** 静的メンバ関数 ** //

	// コピーコンストラクタと演算子オーバーロードの禁止
	SRV(const SRV& obj) = delete;
	SRV& operator=(const SRV& obj) = delete;

	// インスタンスを取得
	static SRV* GetInstance();

	// 開放する
	static void Relese();

public:

	void Initialize(DirectXCommon* dx);
	void CreateShaderResourceView();
	void CreateSRVDescriptorHeap();
	void SetSRVDesc(const std::string filePath);
	int LoadTexture(const std::string filePath);

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
	std::unordered_map<int32_t,TextureData> textureData_;
	// 次に使用可能なテクスチャデータの番地
	int32_t textureId_ = 0;
	int32_t defaultTexId_;

};

