#pragma once
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Append/Animation/Node/NodeAnimation.h"
#include "GameEngine/Append/Animation/Skinning/SkinCluster.h"

struct ModelData;

class Skinnig
{
public: // -- 公開 メンバ関数 -- //

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init(const std::string& directorypath,const std::string& filepath,ModelData modelData);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	// Animationを適用する
	void ApplyAniation();

	SkinCluster GetSkinCluster() { return skinCluster_;}
	Skeleton GetSkeleton() { return skeleton_;}

private: // -- 非公開 メンバ変数 -- //

	// 外部ポインタ
	DirectXCommon* mDxCommon;

	// スキンクラスター
	SkinCluster skinCluster_;
	// スケルトン
	Skeleton skeleton_;

	// アニメーション実行時間
	float animationTime_ = 0.0f;
	// アニメーション
	Animation animation_;

};
