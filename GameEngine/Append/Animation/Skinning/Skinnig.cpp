#include "Skinnig.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "GameEngine/Object/Model/Model.h"

void Skinnig::Init(const std::string& directorypath, const std::string& filepath, ModelData modelData)
{
	mDxCommon = DirectXCommon::GetInstance();

	// スケルトン生成
	mSkeleton = Skeleton::Create(modelData.rootNode);
	
	// 新規にスキンクラスターを含めたデータを生成
	mNowSkincluster = std::make_shared<SkinningStatus>();
	// スキンクラスターを生成
	mNowSkincluster->skinCluster = SkinCluster::Create(mDxCommon->device_, mSkeleton, modelData);
	// アニメーションに必要な情報をセット
	mNowSkincluster->animation = Resource::LoadAnmation(directorypath, filepath);
	// スキンクラスターを生成
	mNowSkincluster->skinCluster = SkinCluster::Create(mDxCommon->device_, mSkeleton, modelData);
	// アニメーションの一時停止をoffに
	mNowSkincluster->isPause = false;
	// ループさせる
	mNowSkincluster->isLoop = true;
	// アニメーションの開始時間を0.0fに設定
	mNowSkincluster->animationTime = 0.0f;
	// アニメーションを有効にする
	mNowSkincluster->isActive = true;

	// モーションブレンドをしていない
	mIsMotionBrending = false;

}

void Skinnig::Update()
{
	// 時刻を進める 右の数値(60.0fはフレームレートに応じて変動させるようにしたい)
	if (mNowSkincluster->isPause == false) {
		mNowSkincluster->animationTime += 1.0f / 60.0f;
	}
	// 最後まで行ったら最初からリピート再生する(しなくてもいいし、フラグで変更しても良さそう)
	if (mNowSkincluster->isLoop == true) {
		mNowSkincluster->animationTime = std::fmod(mNowSkincluster->animationTime, mNowSkincluster->animation.duration);
	}

	// モーションブレンド処理
	if (mIsMotionBrending) {

		// モーションブレンドの進行度を上昇させる
		if (mMotionBrendingTime < 1.0f) 
		{
			mMotionBrendingTime += 1.0f / 60.0f;
		}

		// 上限まで行ったら自動で終了する
		else if (mMotionBrendingTime >= 1.0f)
		{
			mMotionBrendingTime = 1.0f;
			EndMotionBrend();
		}

	}

	// アニメーションの更新を行い、骨ごとのLocal情報を更新する
	ApplyAniation();
	// スケルトン更新
	mSkeleton.Update();
	// スキンクラスター 更新
	mNowSkincluster->skinCluster.Update(mSkeleton);


}

void Skinnig::ApplyAniation() {

	// モーションブレンド中
	if (mIsMotionBrending) 
	{
		for (Joint& joint : mSkeleton.joints)
		{
			// 対象のJointのAnimationがあれば、値の適用を行う。
			// 下記のif文はC++17から可能になった初期化付きif文
			if (auto it = mNowSkincluster->animation.nodeAnimations.find(joint.name); it != mNowSkincluster->animation.nodeAnimations.end())
			{
				if (auto itN = mNextSkincluster->animation.nodeAnimations.find(joint.name); itN != mNextSkincluster->animation.nodeAnimations.end())
				{
					// 両方のルートを取得
					const NodeAnimation& rootAnimation = (*it).second;
					const NodeAnimation& rootAnimationN = (*itN).second;

					// スケール・回転・平行移動を要素ごとに補間を行なう
					std::array<Vector3, 2> transration;
					transration[0] = CalculateValue(rootAnimation.translation, mNowSkincluster->animationTime);
					transration[1] = CalculateValue(rootAnimationN.translation, mNextSkincluster->animationTime);

					std::array<Quaternion, 2> rotation;
					rotation[0] = CalculateValue(rootAnimation.rotation, mNowSkincluster->animationTime);
					rotation[1] = CalculateValue(rootAnimationN.rotation, mNextSkincluster->animationTime);
					
					std::array<Vector3, 2> scale;
					scale[0] = CalculateValue(rootAnimation.scale, mNowSkincluster->animationTime);
					scale[1] = CalculateValue(rootAnimationN.scale, mNextSkincluster->animationTime);

					joint.transform.translation_ = Lerp(transration[0], transration[1], mMotionBrendingTime);
					joint.transform.rotation_ = Slerp(rotation[0], rotation[1], mMotionBrendingTime);
					joint.transform.scale_ = Lerp(scale[0], scale[1], mMotionBrendingTime);
				}
			}

		}
	}
	// 上記以外
	else {
		for (Joint& joint : mSkeleton.joints) 
		{
			// 対象のJointのAnimationがあれば、値の適用を行う。
			// 下記のif文はC++17から可能になった初期化付きif文
			if (auto it = mNowSkincluster->animation.nodeAnimations.find(joint.name); it != mNowSkincluster->animation.nodeAnimations.end()) 
			{
				const NodeAnimation& rootAnimation = (*it).second;
				joint.transform.translation_ = CalculateValue(rootAnimation.translation, mNowSkincluster->animationTime);
				joint.transform.rotation_ = CalculateValue(rootAnimation.rotation, mNowSkincluster->animationTime);
				joint.transform.scale_ = CalculateValue(rootAnimation.scale, mNowSkincluster->animationTime);
			}
		}
	}
}

void Skinnig::CreateSkinningData(const std::string& directorypath, const std::string& filename, const std::string& filrExt, ModelData modelData)
{
	// 新規にスキンクラスターを含めたデータを生成
	std::shared_ptr<SkinningStatus> newSkinning = std::make_shared<SkinningStatus>();
	// スキンクラスターを生成
	newSkinning->skinCluster = SkinCluster::Create(mDxCommon->device_, mSkeleton, modelData);
	// アニメーションに必要な情報をセット
	newSkinning->animation = Resource::LoadAnmation(directorypath, filename + filrExt);
	// スキンクラスターを生成
	newSkinning->skinCluster = SkinCluster::Create(mDxCommon->device_, mSkeleton, modelData);
	// アニメーションの一時停止をoffに
	newSkinning->isPause = false;
	// ループさせる
	newSkinning->isLoop = true;
	// アニメーションの開始時間を0.0fに設定
	newSkinning->animationTime = 0.0f;
	// アニメーションを有効にする
	newSkinning->isActive = true;
	// 名称を設定
	newSkinning->name = filename;

	// 設定したデータをマップに登録する
	mSkinClusterMap[filename] = newSkinning;

	// 上記のshared_ptrはローカルなのでこの時点で参照が外れるのでそのままにしておく
}

void Skinnig::SetNextAnimation(std::string name)
{
	if (mNextSkincluster != nullptr) { return; }

	// アニメーションが存在するか検索する
	std::map<std::string, std::shared_ptr<SkinningStatus>>::iterator it = mSkinClusterMap.find(name);
	// 未登録チェック
	assert(it != mSkinClusterMap.end());

	// 登録されていたら
	// 次回のアニメーションのポインタをシェアする
	mNextSkincluster = mSkinClusterMap.at(name);

	// テスト 設定したら即移行してみる
	StartMotionBrend();

}

void Skinnig::StartMotionBrend()
{
	// 各パラメータの初期化
	mMotionBrendingTime = 0.0f; // モーションブレンド移行時間を0.0fに
	mIsMotionBrending = true; // モーションブレンドのフラグをtrueに
}

void Skinnig::EndMotionBrend()
{
	// 各パラメータの設定
	mIsMotionBrending = false; // モーションブレンドのフラグをfalseに
	mNowSkincluster = mNextSkincluster; // アニメーションを切り替える
	mNextSkincluster = nullptr; // 移行して不要になったアニメーションのポインタを破棄
}
