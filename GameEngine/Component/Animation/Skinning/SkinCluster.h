#pragma once
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Component/Animation/Node/NodeAnimation.h"
#include "GameEngine/Component/Animation/Skinning/Skeleton.h"

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

class SkinCluster
{
public: // -- 公開 メンバ関数 -- //

	// Skinclusterを生成する
	static SkinCluster Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Skeleton& skeleton, const ModelData& modelData);

	// Skinclusterの更新処理を行う
	void Update(const Skeleton& skeleton);

public: // -- 公開 メンバ変数 -- //

	std::vector<Matrix4x4> mInverseBindPoseMatrices;// バインドポーズの逆行列
	Microsoft::WRL::ComPtr<ID3D12Resource> mInfluenceResource;// 頂点に影響を与える(受ける)リソース
	D3D12_VERTEX_BUFFER_VIEW mInfluenceBufferView;// 頂点に影響を与える(受ける)バッファビュー
	std::span<VertexInfluence> mMappedInfluence;

	Microsoft::WRL::ComPtr<ID3D12Resource> mPaletteResource;
	std::span<WellForGPU> mMappedPallete;// Joint数分の行列を保有する
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> mPaletteSrvHandle;

};

