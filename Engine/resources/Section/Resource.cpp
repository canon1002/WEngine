#include "Resource.h"

namespace Resource
{
	// テクスチャ読み込み
	DirectX::ScratchImage LoadTextrue(const std::string& filePath) {
		// テクスチャファイルを読んでプログラムで使えるようにする
		DirectX::ScratchImage image{};
		std::wstring filePathW = WinAPI::ConvertString(filePath);
		HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		assert(SUCCEEDED(hr));

		// ミニマップの作成
		DirectX::ScratchImage mipImages{};
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
		assert(SUCCEEDED(hr));

		// ミニマップ付きのデータを渡す
		return mipImages;
	}

	//テクスチャリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metdate) {
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
		HRESULT hr = device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&resource));
		assert(SUCCEEDED(hr));

		return resource;

	}

	// データを転送する
	void UpdateTextureDate(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
		// Mate情報を取得
		const DirectX::TexMetadata& metadate = mipImages.GetMetadata();
		// 全MipMapについて 
		for (size_t mipLevel = 0; mipLevel < metadate.mipLevels; ++mipLevel) {
			// MipMapを指定して各Imageを取得
			const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
			// Textureに転送
			HRESULT hr = texture->WriteToSubresource(
				UINT(mipLevel),
				nullptr,
				img->pixels,
				UINT(img->rowPitch),
				UINT(img->slicePitch)
			);
			assert(SUCCEEDED(hr));
		}
	}

}