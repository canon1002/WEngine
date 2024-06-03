#pragma once
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Append/Animation/Node/NodeAnimation.h"

struct Joint {
	QuaternionTransform transform;	// Transform情報
	Matrix4x4 localMatrix;			// localatrix
	Matrix4x4 skeletonSpaceMatrix;	// skeletonSpaceでの変換行列
	std::string name;				// 名前
	std::vector<int32_t> childlen;	// 子jointのIndexリスト。いなければ空
	int32_t index;					// 自身のIndex
	std::optional<int32_t> parent;	// 親JointのIndex。いなければnull


	static int32_t Create(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

};
