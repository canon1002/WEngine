#pragma once

#include "Resource/Base/Resource.h"
#include "Component/Animation/Skinning/Joint.h"

// モデルデータ 前方宣言
struct ModelData;

struct Skeleton {
	int32_t root;							// RootJointのIndex
	std::map<std::string, int32_t>jointMap;	// Joint名とindexとの辞書
	std::vector<Joint> joints;				// 所属しているJoint

	// Nodeの階層構造からSkeletonを作る
	static Skeleton Create(const Node& rootNode);

	// スケルトンの更新処理を行う
	void Update();

};