#pragma once
#include "../../math/Math.h"
#include "../../base/DirectXCommon.h"
#include "../../../../externals/DirectXTex/DirectXTex.h" 

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
};

namespace Resource
{	
	// テクスチャ読み込み
	DirectX::ScratchImage LoadTextrue(const std::string& filePath);
	
	//テクスチャリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metdate);

	// データを転送する
	void UpdateTextureDate(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

};

