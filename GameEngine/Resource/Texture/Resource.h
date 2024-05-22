#pragma once

#include "GameEngine/Math/Math.h"
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
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
	Matrix4x4 InverseTransposeWorld;
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

/// <summary>
/// 親子関係を持つNodeクラス
/// </summary>
struct Node {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};

struct VertexWeightData {
	float weight;
	uint32_t vertexIndex;
};
struct JointWeightData {
	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData>vertexWeights;
};

const uint32_t kNumMaxInfluence = 4;
struct VertexInfluence {
	std::array<float, kNumMaxInfluence> weights;
	std::array<int32_t, kNumMaxInfluence> jointIndex;
};

struct WellForGPU {
	Matrix4x4 SkeletonSpaceMatrix; // 位置用
	Matrix4x4 SkeletonSpaceInverseTransposeMatrix; // 法線用
};

struct SkinCluster {
	std::vector<Matrix4x4> inverseBindPoseMatrices;// バインドポーズの逆行列
	Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;// 頂点に影響を与える(受ける)リソース
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView;// 頂点に影響を与える(受ける)バッファビュー
	std::span<VertexInfluence> mappedInfluence;
	Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
	std::span<WellForGPU> mappedPallete;// Joint数分の行列を保有する
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
};


#pragma region アニメーション関連

struct Joint {
	QuaternionTransform transform;	// Transform情報
	Matrix4x4 localMatrix;			// localatrix
	Matrix4x4 skeletonSpaceMatrix;	// skeletonSpaceでの変換行列
	std::string name;				// 名前
	std::vector<int32_t> childlen;	// 子jointのIndexリスト。いなければ空
	int32_t index;					// 自身のIndex
	std::optional<int32_t> parent;	// 親JointのIndex。いなければnull
};

struct Skeleton {
	int32_t root;							// RootJointのIndex
	std::map<std::string, int32_t>jointMap;	// Joint名とindexとの辞書
	std::vector<Joint> joints;				// 所属しているJoint
};

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
	// Nodeの階層構造からSkeletonを作る
	Skeleton CreateSkeleton(const Node& rootNode);
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
	// スケルトンの更新処理を行う
	void UpdateSkeleton(Skeleton& skeleton);
	// Skinclusterの更新処理を行う
	void UpdateSkinCluster(SkinCluster& skinCluster, const Skeleton& skeleton);
	
	// Animationを適用する
	void ApplyAniation(Skeleton& skeleton, const Animation& animation, float animationTime);
}

#pragma endregion

SkinCluster CreateSkinCluster(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
	const Skeleton& skeleton, const ModelData& modelData);

// モデルデータ
struct ModelData {
	std::map<std::string, JointWeightData>skinClusterData;
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
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

