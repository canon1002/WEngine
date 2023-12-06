#pragma once
#include "WinAPI.h"
#include "../../../Externals/DirectXTex/DirectXTex.h"

class DirectXCommon;

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
	void SetSRVDesc0(const char* filePath);
	void SetSRVDesc(const std::string filePath);

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
	//SRVを制作するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU3_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU3_;
	// テクスチャリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2_ = nullptr;
	// リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediaResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediaResource2 = nullptr;

};

