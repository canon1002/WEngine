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
	uint32_t LoadTexture(const std::string& filePath);

	// メタデータ取得
	const DirectX::TexMetadata& GetMetaData(const uint32_t& srvIndex);
	// GPUハンドルの取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const uint32_t& srvIndex);

	// MipImageの生成
	DirectX::ScratchImage CreateMipImages(const std::string& filePath);

	//テクスチャリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(
		Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metdate);

	void UpdateTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);
	Microsoft::WRL::ComPtr<ID3D12Resource>UpdateTextureData(Microsoft::WRL::ComPtr < ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);


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

	// ディレクトリパス
	const std::string kDirectoryPath = "Resources/objs/";

	// テクスチャデータ
	struct TextureData {
		std::string filePath; // ファイルパス
		DirectX::TexMetadata metadata;// 画像ファイルの情報
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;// テクスチャリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediaResource;// 中間テクスチャリソース
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;// SRV作成時に必要なCPUハンドル
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;// 描画コマンドに必要なGPUハンドル
	};

	// テクスチャデータのマップ
	std::unordered_map<uint32_t, TextureData> mTextureData;

};

