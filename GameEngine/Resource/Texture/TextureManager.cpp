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

	// デフォルトテクスチャの読み込み
	mDefaultTextureIndex = LoadTexture("white2x2.dds");

}

uint32_t TextureManager::LoadTexture(const std::string& filePath)
{
	// 読み込み済みテクスチャを検索
	for (auto& texture : mTextureData) {
		if (texture.second.filePath == filePath) {
			// 読み込み済みならそのままインデックスを返す
			return texture.first;
		}
	}

	// SRVマネージャのポインタを取得
	SRV* srvManager = DirectXCommon::GetInstance()->mSrv.get();

	// テクスチャ枚数上限チェック
	assert(srvManager->mUseIndex < srvManager->kMaxSRVCount);

	// srvインデックスを確保
	uint32_t srvIndex = srvManager->Allocate();
	// テクスチャデータを追加
	mTextureData.insert(std::make_pair(srvIndex, TextureData{}));

	// テクスチャデータを参照して書き込む
	TextureData& textureData = mTextureData[srvIndex];
	textureData.filePath = filePath;
	// テクスチャを読んで転送する
	DirectX::ScratchImage mipImages = CreateMipImages(filePath);
	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = CreateTextureResource(DirectXCommon::GetInstance()->mDevice, textureData.metadata);
	textureData.intermediaResource = UpdateTextureData(textureData.resource, mipImages);
	textureData.srvHandleCPU = srvManager->GetCPUDescriptorHandle(srvIndex);
	textureData.srvHandleGPU = srvManager->GetGPUDescriptorHandle(srvIndex);

	// SRVの作成
	if (textureData.metadata.IsCubemap()) {
		// キューブマップ用の設定
		srvManager->CreateSRVforTextureCubeMap(
			srvIndex, textureData.resource.Get(), textureData.metadata.format);
	}
	else {
		// 今まで通り2Dテクスチャの設定を行う
		srvManager->CreateSRVforTexture2D(
			srvIndex, textureData.resource.Get(),
			textureData.metadata.format, UINT(textureData.metadata.mipLevels));
	}

	// 最後にインデックスを返す
	return srvIndex;
}



const DirectX::TexMetadata& TextureManager::GetMetaData(const uint32_t& srvIndex)
{
	TextureData& textureData = mTextureData.at(srvIndex);
	return textureData.metadata;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const uint32_t& srvIndex)
{
	TextureData& textureData = mTextureData.at(srvIndex);
	return textureData.srvHandleGPU;
}

DirectX::ScratchImage TextureManager::CreateMipImages(const std::string& filePath)
{
	// テクスチャファイルを読んでプログラムで使えるようにする
	DirectX::ScratchImage image{};
	const std::string& fullPath = kDirectoryPath + filePath;
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

// データを転送する
void TextureManager::UpdateTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
	// Mate情報を取得
	const DirectX::TexMetadata& metadate = mipImages.GetMetadata();
	// 全MipMapについて 
	for (size_t mipLevel = 0; mipLevel < metadate.mipLevels; ++mipLevel) {
		// MipMapを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		// Textureに転送
		HRESULT hr;
		hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)
		);
		assert(SUCCEEDED(hr));
	}
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UpdateTextureData(Microsoft::WRL::ComPtr <ID3D12Resource> texture, const DirectX::ScratchImage& mipImages)
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(DirectXCommon::GetInstance()->mDevice.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
	Microsoft::WRL::ComPtr <ID3D12Resource> intermediateResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), intermediateSize);
	UpdateSubresources(DirectXCommon::GetInstance()->mCommandList.Get(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
	// Textureの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPYからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;// Noneにしておく
	barrier.Transition.pResource = texture.Get();// バリアを張る対象の
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;		// 遷移前(現在)のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;	// 遷移後のResourceState
	DirectXCommon::GetInstance()->mCommandList->ResourceBarrier(1, &barrier);		// TransitionBarrierを張る
	return intermediateResource;
}