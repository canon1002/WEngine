#include "Skinnig.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "GameEngine/Object/Model/Model.h"

void Skinnig::Init(const std::string& directorypath, const std::string& filepath, ModelData modelData)
{
	dxCommon_ = DirectXCommon::GetInstance();

	// アニメーションに必要な情報をセット
	animation_ = Resource::LoadAnmation(directorypath, filepath);
	
	// スケルトン生成
	skeleton_ = Skeleton::Create(modelData.rootNode);
	
	// スキンクラスター生成
	skinCluster_ = SkinCluster::Create(dxCommon_->device_, skeleton_, modelData);

}

void Skinnig::Update()
{
	// 時刻を進める 右の数値(60.0fはフレームレートに応じて変動させるようにしたい)
	animationTime_ += 1.0f / 60.0f;
	// 最後まで行ったら最初からリピート再生する(しなくてもいいし、フラグで変更しても良さそう)
	animationTime_ = std::fmod(animationTime_, animation_.duration);
	// アニメーションの更新を行い、骨ごとのLocal情報を更新する
	ApplyAniation();
	// スケルトン更新
	skeleton_.Update();
	// スキンクラスター 更新
	skinCluster_.Update(skeleton_);
}

void Skinnig::ApplyAniation() {
	for (Joint& joint : skeleton_.joints) {
		// 対象のJointのAnimationがあれば、値の適用を行う。
		// 下記のif文はC++17から可能になった初期化付きif文
		if (auto it = animation_.nodeAnimations.find(joint.name); it != animation_.nodeAnimations.end()) {
			const NodeAnimation& rootAnimation = (*it).second;
			joint.transform.translation_ = CalculateValue(rootAnimation.translation, animationTime_);
			joint.transform.rotation_ = CalculateValue(rootAnimation.rotation, animationTime_);
			joint.transform.scale_ = CalculateValue(rootAnimation.scale, animationTime_);
		}
	}
}
