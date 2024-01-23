#pragma once
#include "../../math/Math.h"
#include "../../color/Color.h"
#include "../../base/DirectXCommon.h"
#include "../../../../externals/DirectXTex/DirectXTex.h" 
#include <vector>
#include "../../../../externals/DirectXTex/d3dx12.h"

struct VertexData {
	Vec4 position;
	Vec2 texcoord;
	Vec3 normal;
};

struct Material {
	Color color;
	int32_t enableLighting;
	float paddding[3];
	Mat44 uvTransform;
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

