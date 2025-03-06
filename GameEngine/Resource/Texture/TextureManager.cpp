#include "TextureManager.h"

// staticメンバ変数で宣言したインスタンスを初期化
TextureManager* TextureManager::instance = nullptr;

TextureManager* TextureManager::GetInstance() {

	// 関数内staticは初めて通ったときのみ実行される
	if (instance == nullptr) {
		instance = new TextureManager;
	}
	return instance;
}

void TextureManager::Final() {
	delete instance;
	instance = nullptr;
}

void TextureManager::Init() {

	// SRVの数と同数
	mTextureData.reserve(SRV::kMaxSRVCount);

}

void TextureManager::LoadTexture(const std::string& filePath)
{
	// 読み込み済みテクスチャを検索
	if (mTextureData.contains(filePath)) {
		
		// 読み込み済みなら早期リターン
		return;
	}

	// SRVマネージャのポインタを取得
	SRV* srvManager = DirectXCommon::GetInstance()->mSrv.get();

	// テクスチャ枚数上限チェック
	assert(srvManager->mUseIndex >= srvManager->kMaxSRVCount);

	// テクスチャデータを追加
	mTextureData.insert(std::make_pair(filePath, TextureData{}));

	// テクスチャデータを参照して書き込む
	TextureData& textureData = mTextureData[filePath];

	// SRV確保
	textureData.srvIndex = srvManager->Allocate();
	textureData.metadata = CreateMipImages(filePath).GetMetadata();
	textureData.resource = CreateTextureResource(DirectXCommon::GetInstance()->mDevice, textureData.metadata);
	textureData.srvHandleCPU = srvManager->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager->GetGPUDescriptorHandle(textureData.srvIndex);

	// SRVの作成
	if (textureData.metadata.IsCubemap()) {
		// キューブマップ用の設定
		srvManager->CreateSRVforTextureCubeMap(
			textureData.srvIndex, textureData.resource.Get(), textureData.metadata.format);
	}
	else {
		// 今まで通り2Dテクスチャの設定を行う
		srvManager->CreateSRVforTexture2D(
			textureData.srvIndex, textureData.resource.Get(),
			textureData.metadata.format, UINT(textureData.metadata.mipLevels));
	}


}

void TextureManager::SetInstancingBuffer(int32_t kNumInstance, Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource){
	
	// SRVマネージャのポインタを取得
	SRV* srvManager = DirectXCommon::GetInstance()->mSrv.get();
	// テクスチャ枚数上限チェック
	assert(srvManager->mUseIndex >= srvManager->kMaxSRVCount);

	// テクスチャデータを追加
	mTextureData.insert(std::make_pair(filePath, TextureData{}));

	// テクスチャデータを参照して書き込む
	TextureData& textureData = mTextureData[filePath];
	// インスタンシング用のSRVを設定
	srvManager->CreateSRVforStructuredBuffer(, instancingResource.Get(), kNumInstance, sizeof(ParticleForGPU));
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath)
{
	TextureData& textureData = mTextureData.at(filePath);
	return textureData.metadata;
}

uint32_t TextureManager::GetSrvIndex(const std::string& filePath = "white2x2.png")
{
	TextureData& textureData = mTextureData.at(filePath);
	return textureData.srvIndex;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath)
{
	TextureData& textureData = mTextureData.at(filePath);
	return textureData.srvHandleGPU;
}

DirectX::ScratchImage TextureManager::CreateMipImages(const std::string& filePath)
{
	// テクスチャファイルを読んでプログラムで使えるようにする
	DirectX::ScratchImage image{};
	const std::string& fullPath = "Resources/objs/" + filePath;
	std::wstring filePathW = WinApp::ConvertString(fullPath);

	// .ddsで終わっていれば.ddsとみなす。別の方法も存在するらしい
	HRESULT hr;
	if (filePathW.ends_with(L".dds")) {
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else {
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}
	assert(SUCCEEDED(hr));

	// ミニマップの作成
	DirectX::ScratchImage mipImages{};
	// 圧縮データかどうか確認する
	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		// 圧縮データであればそのまま使うのでmoveする
		mipImages = std::move(image);
	}
	else {
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(),
			DirectX::TEX_FILTER_SRGB, 0, mipImages);
	}
	assert(SUCCEEDED(hr));

	// 生成したミップマップを返す
	return mipImages;
}


Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(
	Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metdate) {

	//metadateを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metdate.width);
	resourceDesc.Height = UINT(metdate.height);
	resourceDesc.MipLevels = UINT16(metdate.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metdate.arraySize);
	resourceDesc.Format = metdate.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metdate.dimension);

	// 利用するヒープの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;// 細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// リソースを生成する
	Microsoft::WRL::ComPtr <ID3D12Resource> resource = nullptr;
	HRESULT hr;
	hr = device->CreateCommittedResource(
		&heapProperties,// Heapの設定
		D3D12_HEAP_FLAG_NONE,// Heapの特殊な設定(特になし)
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,// データ転送される設定
		nullptr,// Crear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));// 作成するresourceポインタへのポインタ
	assert(SUCCEEDED(hr));

	return resource;

}