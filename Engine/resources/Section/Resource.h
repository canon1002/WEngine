#pragma once
#include "../../math/Math.h"
#include "../../base/DirectXCommon.h"
#include "../../../../externals/DirectXTex/DirectXTex.h" 
#include <vector>
#include "../../../../externals/DirectXTex/d3dx12.h"

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
};

namespace Resource
{	
	// テクスチャ読み込み
	DirectX::ScratchImage LoadTextrue(const std::string& filePath);
	
	//テクスチャリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(
		Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metdate);

	//ディープステンシルテクスチャリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDeapStencilTextureResource(
		Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height);

	// データを転送する
	void UpdateTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);


	Microsoft::WRL::ComPtr<ID3D12Resource>UpdateTextureData(
		Microsoft::WRL::ComPtr < ID3D12Resource> texture,
		const DirectX::ScratchImage& mipImages,
		DirectXCommon* dx);

};

