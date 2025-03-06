#pragma once
#include "Resource.h"

class TextureManager
{
public: // -- 公開 メンバ関数 -- // 

	// インスタンス取得
	static TextureManager* GetInstance();
	// 終了処理
	void Final();
	// 初期化処理
	void Init();

	// テクスチャの読み込み
	void LoadTexture(const std::string& filePath);
	// テクスチャの読み込み(インスタンス数指定)
	void SetInstancingBuffer(int32_t kNumInstance, Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource);

	// メタデータ取得
	const DirectX::TexMetadata& GetMetaData(const std::string& filePath);
	// SRVインデックスの取得
	uint32_t GetSrvIndex(const std::string& filePath = "white2x2.png");
	// GPUハンドルの取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);

	// MipImageの生成
	DirectX::ScratchImage CreateMipImages(const std::string& filePath);

	//テクスチャリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(
		Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metdate);

private: // -- 非公開 メンバ関数 -- //
	
	// コンストラクタ
	TextureManager() = default;
	// デストラクタ
	~TextureManager() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	TextureManager(const TextureManager& obj) = delete;
	TextureManager& operator=(const TextureManager& obj) = delete;


public: // -- 公開 メンバ変数 -- //

	// デフォルトテクスチャの番地
	uint32_t mDefaultTextureIndex;

private: // -- 非公開 メンバ変数 -- //

	// インスタンス
	static TextureManager* instance;

	// テクスチャデータ
	struct TextureData {
		uint32_t srvIndex; // テクスチャを保有するSRV番地
		DirectX::TexMetadata metadata;// 画像ファイルの情報
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;// テクスチャリソース
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;// SRV作成時に必要なCPUハンドル
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;// 描画コマンドに必要なGPUハンドル
	};

	// テクスチャデータのマップ
	std::unordered_map<std::string, TextureData> mTextureData;

};

