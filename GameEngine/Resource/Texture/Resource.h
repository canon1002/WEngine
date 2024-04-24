#pragma once

#include "GameEngine/Math/Math.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Material/Material.h"

//
#include <vector>

// 外部ファイル参照
#include "Externals/DirectXTex/d3dx12.h"
#include "Externals/DirectXTex/DirectXTex.h"

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Node{
	Mat44 localMatrix;
	std::string name;
	std::vector<Node> children;
};

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
	Node rootNode;
};

/// <summary>
/// UVTransform用の構造体
/// <para> Vector3型の [スケール・回転・平行移動] を保持する </para>
/// </summary>
struct UVTransform{
	Vec3 scale;
	Vec3 rotation;
	Vec3 translation;
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
		DirectXCommon* dxCommon,
		Microsoft::WRL::ComPtr < ID3D12Resource> texture,
		const DirectX::ScratchImage& mipImages);

	/// <summary>
	///	Assimpで3Dモデルの読み込みを行う関数
	/// </summary>
	/// <param name="directoryPath">ディレクトリパス</param>
	/// <param name="filename">読み込むモデルのファイル名</param>
	/// <returns>読み込んだモデルデータを返す</returns>
	ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// AssimpのNode(aiNode)を自作のNode構造体に変換する関数
	/// </summary>
	/// <param name="node">AssimpのNode(aiNode)</param>
	/// <returns>Node</returns>
	Node ReadNode(aiNode* node);

	// .objの読み込み
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
	// .mtlの読み込み
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

};

