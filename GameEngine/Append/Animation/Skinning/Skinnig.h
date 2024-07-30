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

	// スキンクラスターの取得
	SkinCluster GetSkinCluster() { return mSkinCluster;}

	// スケルトンの取得
	Skeleton GetSkeleton() { return mSkeleton;}

	// 一時停止する
	void Pause() { mIsPauseAnimation = true; }

	// アニメーションを再生する(一時停止やアニメーション停止も解除する)
	void Play() { mIsPauseAnimation = false; }

	// ループするかの設定(trueだとループする)
	void SetLoopMode(bool isLoop) { mIsLoop = isLoop; }
	void ResetTime() { mAnimationTime = 0.0f; }
	bool IsAnimationFinished()const  {
		if (mAnimationTime >= (mAnimation.duration/2.0f)) {
			return true;
		}
		return false;
	}

private: // -- 非公開 メンバ変数 -- //

	// 外部ポインタ
	DirectXCommon* mDxCommon;

	// スキンクラスター
	SkinCluster mSkinCluster;
	// スケルトン
	Skeleton mSkeleton;

	// アニメーション実行時間
	float mAnimationTime = 0.0f;
	// アニメーション
	Animation mAnimation;

	// アニメーション 一時停止
	bool mIsPauseAnimation;

	// アニメーションのループ設定
	bool mIsLoop = true;

};
