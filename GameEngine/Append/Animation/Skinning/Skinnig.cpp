#include "Skinnig.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "GameEngine/Object/Model/Model.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/GameMaster/Framerate.h"

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
	// モーションブレンドの切替速度(遅い n ~ 1.0f ~ m 早い)
	mMotionBrendingInterval = 1.0f;

	// アニメーションの再生速度
	mAnimationPlaySpeed = 1.0f;

}

void Skinnig::Update()
{
	// 時刻を進める
	if (mNowSkincluster->isPause == false) {
		mNowSkincluster->animationTime += (mAnimationPlaySpeed / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetBattleSpeed();
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
			mMotionBrendingTime += (mMotionBrendingInterval / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetBattleSpeed();
		}

		// 上限まで行ったら自動で終了する
		else if (mMotionBrendingTime >= 1.0f)
		{
			mMotionBrendingTime = 1.0f;
			EndMotionBrend();
		}
		if (!mWaitingSkinClusters.empty()) {
			mWaitingSkinClusters.front()->skinCluster.Update(mSkeleton);
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
				if (auto itNext = mWaitingSkinClusters.front()->animation.nodeAnimations.find(joint.name); itNext != mWaitingSkinClusters.front()->animation.nodeAnimations.end())
				{
					// 両方のルートを取得
					const NodeAnimation& rootAnimation = (*it).second;
					const NodeAnimation& rootAnimationNext = (*itNext).second;

					// スケール・回転・平行移動を要素ごとに補間を行なう

					// 各アニメーションの平行移動をVector3に変換し、取得
					std::array<Vector3, 2> transration;
					transration[0] = CalculateValue(rootAnimation.translation, mNowSkincluster->animationTime);
					transration[1] = CalculateValue(rootAnimationNext.translation, mWaitingSkinClusters.front()->animationTime);

					// 各アニメーション回転をQuaternionに変換し、取得
					std::array<Quaternion, 2> rotation;
					rotation[0] = CalculateValue(rootAnimation.rotation, mNowSkincluster->animationTime);
					rotation[1] = CalculateValue(rootAnimationNext.rotation, mWaitingSkinClusters.front()->animationTime);

					// 各アニメーションのスケールをVector3に変換し、取得
					std::array<Vector3, 2> scale;
					scale[0] = CalculateValue(rootAnimation.scale, mNowSkincluster->animationTime);
					scale[1] = CalculateValue(rootAnimationNext.scale, mWaitingSkinClusters.front()->animationTime);

					// 線形補間(回転は球面線形補間)を行いモーションブレンドする
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

void Skinnig::DrawGui()
{
#ifdef _DEBUG

	ImGui::TextColored(ImVec4(1.0f,0.6f,0.6f,1.0f), mNowSkincluster->name.c_str());

	ImGui::DragFloat("AnimationPlaySpeed", &mAnimationPlaySpeed, 0.1f, -2.0f, 10.0f);

	if (mIsMotionBrending) {
		ImGui::Text("MotionBrending: [ON]");
	}
	else {
		ImGui::Text("MotionBrending: [OFF]");
	}

	ImGui::DragFloat("MotionBrendingInterval", &mMotionBrendingInterval, 0.1f, 1.0f, 60.0f);


	ImGui::DragFloat("animationTime", &mNowSkincluster->animationTime);
	ImGui::DragFloat("duration", &mNowSkincluster->animation.duration, 0.0f);

	if (mNowSkincluster->isLoop) {
		ImGui::Text("Loop: [ON]");
	}
	else {
		ImGui::Text("Loop: [OFF]");
	}


	// 待機状態のアニメーションの情報
	int32_t count = 0;
	for (auto& skinCluster : mWaitingSkinClusters) {
		std::stringstream ss;
		ss << std::setfill('0') << std::setw(3) << ++count << skinCluster->name;

		if (ImGui::TreeNode(ss.str().c_str()))
		{
			ImGui::DragFloat("animationTime", &skinCluster->animationTime, 0.0f);
			ImGui::DragFloat("duration", &skinCluster->animation.duration, 0.0f);

			if (skinCluster->isLoop) {
				ImGui::Text("Loop: [ON]");
			}
			else {
				ImGui::Text("Loop: [OFF]");
			}

			ImGui::TreePop();
		}
	}

	// 登録済みのモーションデータ
	if(ImGui::CollapsingHeader("AllMotionData") ){
		for (auto& skinCluster : mSkinClusterMap) {

			if (ImGui::TreeNode(skinCluster.second->name.c_str()))
			{
				ImGui::DragFloat("animationTime", &skinCluster.second->animationTime, 0.0f);
				ImGui::DragFloat("duration", &skinCluster.second->animation.duration, 0.0f);

				if (skinCluster.second->isLoop) {
					ImGui::Text("Loop: [ON]");
				}
				else {
					ImGui::Text("Loop: [OFF]");
				}

				ImGui::TreePop();
			}
		}
	}

#endif // _DEBUG
}

void Skinnig::CreateSkinningData(const std::string& directorypath, const std::string& filename, const std::string& filrExt, ModelData modelData, bool isLoop)
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
	newSkinning->isLoop = isLoop;
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
	// アニメーションが存在するか検索する
	std::map<std::string, std::shared_ptr<SkinningStatus>>::iterator it = mSkinClusterMap.find(name);
	// 未登録チェック
	assert(it != mSkinClusterMap.end());

	// 登録されていたら
	// 切り替え待ちの配列に追加(ポインタのシェア)
	mWaitingSkinClusters.push_back(mSkinClusterMap.at(name));

	StartMotionBrend();
}

void Skinnig::StartMotionBrend()
{
	// 各パラメータの初期化
	mMotionBrendingTime = 0.0f; // モーションブレンド移行時間を0.0fに
	mIsMotionBrending = true; // モーションブレンドのフラグをtrueに

	// 次回アニメーションの開始時間を設定
	mWaitingSkinClusters.front()->animationTime = 0.0f;

}

void Skinnig::EndMotionBrend()
{
	// 各パラメータの設定
	mIsMotionBrending = false; // モーションブレンドのフラグをfalseに
	mNowSkincluster = mWaitingSkinClusters.front(); // アニメーションを切り替える
	mWaitingSkinClusters.erase(mWaitingSkinClusters.begin()); // 移行して不要になったアニメーションのポインタを破棄

	// 配列がからでなければ再実行
	if (!mWaitingSkinClusters.empty()) {
		StartMotionBrend();
	}

}

bool Skinnig::GetIsActiveAnimation(std::string name)
{
	if (mNowSkincluster->name.c_str() == name.c_str()) {
		return true;
	}

	return false;
}

bool Skinnig::SearchToWaitingSkinCluster(std::string name)
{
	for (auto& skinCluster : mWaitingSkinClusters) {
		if (skinCluster->name == name) {
			return true;
		}
	}
	return false;
}
