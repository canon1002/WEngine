#pragma once

#include "Engine/Math/Math.h"
#include "Engine/Base/DirectXCommon.h"
#include "Engine/Object/Texture/Material.h"

//
#include <vector>

// 外部ファイル参照
#include "Externals/DirectXTex/d3dx12.h"
#include "Externals/DirectXTex/DirectXTex.h"

struct VertexData {
	Vec4 position;
	Vec2 texcoord;
	Vec3 normal;
};

struct VertexData2D {
	Vec4 position;
	Vec2 texcoord;
};


struct TransformationMatrix {
	Mat44 WVP;
	Mat44 World;
};

struct ParticleForGPU {
	Mat44 WVP;
	Mat44 World;
	Color color;
};

struct CameraForGPU
{
	Vec3 worldPosition;
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
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
		const DirectX::ScratchImage& mipImages);

	// .objの読み込み
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
	// .mtlの読み込み
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

};

