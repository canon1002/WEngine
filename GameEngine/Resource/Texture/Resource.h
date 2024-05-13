#pragma once

#include "GameEngine/Math/Math.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Material/Material.h"

// stl
#include <vector>
#include <map>

// 外部ファイル参照
#include "Externals/DirectXTex/d3dx12.h"
#include "Externals/DirectXTex/DirectXTex.h"

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "GameEngine/Math/Math.h"

/// <summary>
/// 親子関係を持つNodeクラス
/// </summary>
struct Node{
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};

// 頂点データ
struct VertexData {
	Vector4 position;
	Vec2 texcoord;
	Vector3 normal;
};
// 2D用 頂点データ
struct VertexData2D {
	Vector4 position;
	Vec2 texcoord;
};
// 
struct TransformationMatrix {
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
// モデルデータ
struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
	Node rootNode;
};

/// <summary>
/// UVTransform用の構造体
/// <para> Vector3型の [スケール・回転・平行移動] を保持する </para>
/// </summary>
struct UVTransform {
	Vector3 scale;
	Vector3 rotation;
	Vector3 translation;
};

#pragma region アニメーション関連

// templateを用いて<Vector3型>と<Quaternion型>のキーフレーム構造体を作成
template <typename tValue>
struct Keyframe {
	float time;		// 値キーフレームの時刻(単位は秒)
	tValue value;	// キーフレームの値
};
using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

struct NodeAnimation {
	std::vector<KeyframeVector3> translation;
	std::vector<KeyframeQuaternion> rotation;
	std::vector<KeyframeVector3> scale;
};


// アニメーション構造体
struct Animation {
	float duration; // アニメーション全体の尺 (単位は秒)
	// NodeAnimationの集合 Node名でひけるようにしておく
	std::map<std::string, NodeAnimation> nodeAnimations;
};

namespace Animations {

	// 任意の時刻の値を取得する関数(Vector3型)
	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
	// 任意の時刻の値を取得する関数(Quaternion型)
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

}

#pragma endregion

namespace Resource // ここから関数の宣言と定義を行う
{	
	// テクスチャ読み込み
	DirectX::ScratchImage LoadTextrue(const std::string& filePath);
	
	//テクスチャリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(
		Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metdate);

	//ディープステンシルテクスチャリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDeapStencilTextureResource(
		Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height);

	// レンダーテクスチャの生成
	Microsoft::WRL::ComPtr <ID3D12Resource> CreateRenderTextureResource(
		Microsoft::WRL::ComPtr <ID3D12Device> device, int32_t width, int32_t height,
		DXGI_FORMAT format, const Vector4& clearColor
	);

	// データを転送する
	void UpdateTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);


	Microsoft::WRL::ComPtr<ID3D12Resource>UpdateTextureData(
		DirectXCommon* dxCommon,
		Microsoft::WRL::ComPtr < ID3D12Resource> texture,
		const DirectX::ScratchImage& mipImages);

	/// <summary>
	///	Assimpで3Dモデルの読み込みを行う関数
	/// </summary>
	/// <param name="directoryPath">ディレクトリパス "Resources/objs/"を記載すること </param>
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

	// アニメーション読み込み
	// 現時点では最初のアニメーションのみ対応できる 必要になったら処理を追加すること
	Animation LoadAnmation(const std::string& directoryPath, const std::string& filePath);

};

