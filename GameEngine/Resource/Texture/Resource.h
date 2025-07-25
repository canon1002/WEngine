#pragma once

#include "GameEngine/Utility/Math/Math.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Material/Material.h"

// stl
#include <vector>
#include <map>
#include <optional>
#include <array>
#include <span>

// 外部ファイル参照
#include "Externals/DirectXTex/d3dx12.h"
#include "Externals/DirectXTex/DirectXTex.h"

// assimp
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

// 頂点データ
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

// 2D用 頂点データ
struct VertexData2D {
	Vector4 position;
	Vector2 texcoord;
};
// 
struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 InverseTransposeWorld;
};

struct TransformationMatrixForGrid3D {
	Matrix4x4 WVP;
	Matrix4x4 World;
};
// パーティクル用
struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Color color;
};
// カメラ
struct CameraForGPU
{
	Vector3 worldPosition;
};
// マテリアルデータ
struct MaterialData {
	std::string textureFilePath; 
};


// 前方宣言
struct Animation;
struct ModelData;
struct MultiModelData;
struct JointWeightData;

/// <summary>
/// UVTransform用の構造体
/// <para> Vector3型の [スケール・回転・平行移動] を保持する </para>
/// </summary>
struct UVTransform {
	Vector3 scale;
	Vector3 rotation;
	Vector3 translation;
};


namespace Resource // ここから関数の宣言と定義を行う
{	
	//ディープステンシルテクスチャリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDeapStencilTextureResource(
		Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height);

	// レンダーテクスチャの生成
	Microsoft::WRL::ComPtr <ID3D12Resource> CreateRenderTextureResource(
		Microsoft::WRL::ComPtr <ID3D12Device> device, uint32_t width, uint32_t height,
		DXGI_FORMAT format, const Vector4& clearColor
	);


	/// <summary>
	///	Assimpで3Dモデルの読み込みを行う関数
	/// </summary>
	/// <param name="directoryPath">ディレクトリパス "Resources/objs/"を記載すること </param>
	/// <param name="filename">読み込むモデルのファイル名</param>
	/// <returns>読み込んだモデルデータを返す</returns>
	ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);
	MultiModelData LoadMultiModelFile(const std::string& directoryPath, const std::string& filename);


	//void ExportModelFile(const std::string& directoryPath, const std::string& filename,Animation* animation);

	// .objの読み込み
	//ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
	// .mtlの読み込み
	//MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	// アニメーション読み込み
	// 現時点では最初のアニメーションのみ対応できる 必要になったら処理を追加すること
	Animation LoadAnmation(const std::string& directoryPath, const std::string& filePath);

	// アニメーション読み込み
	// 複数のアニメーションに対応できるようにする
	Animation LoadAnmation(const std::string& directoryPath, const std::string& filePath, std::string animationName);

	void LoadAnimations(const std::string& directoryPath, const std::string& filePath, std::vector<Animation> vector);

};

