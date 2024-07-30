#include "Skinnig.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "GameEngine/Object/Model/Model.h"

void Skinnig::Init(const std::string& directorypath, const std::string& filepath, ModelData modelData)
{
	mDxCommon = DirectXCommon::GetInstance();

	// アニメーションに必要な情報をセット
	mAnimation = Resource::LoadAnmation(directorypath, filepath);
	
	// スケルトン生成
	mSkeleton = Skeleton::Create(modelData.rootNode);
	
	// スキンクラスター生成
	mSkinCluster = SkinCluster::Create(mDxCommon->device_, mSkeleton, modelData);

	mIsPauseAnimation = false;
	mIsLoop = true;
	mAnimationTime = 0.0f;
}

void Skinnig::Update()
{
	// 時刻を進める 右の数値(60.0fはフレームレートに応じて変動させるようにしたい)
	if (mIsPauseAnimation == false) {
		mAnimationTime += 1.0f / 60.0f;
	}
	// 最後まで行ったら最初からリピート再生する(しなくてもいいし、フラグで変更しても良さそう)
	if (mIsLoop == true) {
		mAnimationTime = std::fmod(mAnimationTime, mAnimation.duration);
	}
	// アニメーションの更新を行い、骨ごとのLocal情報を更新する
	ApplyAniation();
	// スケルトン更新
	mSkeleton.Update();
	// スキンクラスター 更新
	mSkinCluster.Update(mSkeleton);


}

void Skinnig::ApplyAniation() {
	for (Joint& joint : mSkeleton.joints) {
		// 対象のJointのAnimationがあれば、値の適用を行う。
		// 下記のif文はC++17から可能になった初期化付きif文
		if (auto it = mAnimation.nodeAnimations.find(joint.name); it != mAnimation.nodeAnimations.end()) {
			const NodeAnimation& rootAnimation = (*it).second;
			joint.transform.translation_ = CalculateValue(rootAnimation.translation, mAnimationTime);
			joint.transform.rotation_ = CalculateValue(rootAnimation.rotation, mAnimationTime);
			joint.transform.scale_ = CalculateValue(rootAnimation.scale, mAnimationTime);
		}
	}
}
