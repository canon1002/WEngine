#pragma once
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Append/Animation/Node/NodeAnimation.h"
#include "GameEngine/Append/Animation/Skinning/SkinCluster.h"

struct ModelData;

// アニメーションに際しての設定パラメータ
struct SkinningStatus {
	std::string name; // Mapに登録された名称(確認用)
	SkinCluster skinCluster;	// スキンクラスター
	float animationTime;	// アニメーション実行時間
	Animation animation;	// アニメーションデータ
	bool isPause;	// 一時停止の設定
	bool isLoop;	//  ループ設定
	bool isActive; // アニメーションを有効にするか(骨のみ使う場合はfalse)
};

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
	SkinCluster GetSkinCluster() { return mNowSkincluster->skinCluster;}

	// スケルトンの取得
	Skeleton GetSkeleton() { return mSkeleton;}

	// ImGuiの表示
	void DrawGui();

	// 一時停止する
	void Pause() { mNowSkincluster->isPause = true; }

	// アニメーションを再生する(一時停止やアニメーション停止も解除する)
	void Play() { mNowSkincluster->isPause = false; }

	// ループするかの設定(trueだとループする)
	void SetLoopMode(bool isLoop) { mNowSkincluster->isLoop = isLoop; }
	void ResetTime() { mNowSkincluster->animationTime = 0.0f; }
	bool IsAnimationFinished()const  {
		if (mNowSkincluster->animationTime >= (mNowSkincluster->animation.duration/2.0f)) {
			return true;
		}
		return false;
	}

	bool GetIsActive() { return mNowSkincluster->isActive; }
	void IsInactive() { mNowSkincluster->isActive = false; }


	// -- モーションブレンド関連 -- //


	/// <summary>
	/// スキニングで必要なデータを生成し、マップに登録する
	/// </summary>
	/// <param name="directorypath">ディレクトリパス</param>
	/// <param name="filename">リソースファイルの名称</param>
	/// <param name="filrExt">ファイル拡張子</param>
	/// <param name="modelData">モデルデータ</param>
	/// <param name="isLoop">ループするか</param>
	void CreateSkinningData(const std::string& directorypath, const std::string& filename, const std::string& filrExt,
		ModelData modelData, bool isLoop = false);

	/// <summary>
	/// 移行後のモーションを設定する
	/// </summary>
	/// <param name="name"></param>
	void SetNextAnimation(std::string name);

	/// <summary>
	/// モーションブレンドを開始する
	/// <para> 次回のアニメーションが設定されていない場合は停止する </para>
	/// </summary>
	void StartMotionBrend();

	/// <summary>
	/// モーションブレンドの終了
	/// </summary>
	void EndMotionBrend();

	float GetMotionBlendingTime() { return mMotionBrendingTime; }
	void SetMotionBlendingInterval(float interval) { mMotionBrendingInterval = interval; }

	float GetDurationTime() { return mNowSkincluster->animation.duration; }
	float GetDurationTime(std::string name) { return mSkinClusterMap.at(name)->animation.duration; }

	bool GetIsMotionbrending() { return mIsMotionBrending; }
	bool GetIsActiveAnimation(std::string name);

	std::shared_ptr<SkinningStatus> GetNowSkinCluster() { return mNowSkincluster; }

	void SetAnimationPlaySpeed(float speed) { mAnimationPlaySpeed = speed; }

	std::vector<std::shared_ptr<SkinningStatus>> GetWaitingSkinClusters() { return mWaitingSkinClusters; }
	bool SearchToWaitingSkinCluster(std::string name);

private: // -- 非公開 メンバ変数 -- //

	// 外部ポインタ
	DirectXCommon* mDxCommon;

	// スケルトン
	Skeleton mSkeleton;

	// 現在行っているアニメーションのスキンクラスター
	std::shared_ptr<SkinningStatus> mNowSkincluster;

	// スキンクラスター(及びフラグ等)のリスト(モーションブレンド等で使用する)
	std::map<std::string, std::shared_ptr<SkinningStatus>>mSkinClusterMap;
	// 次に行うアニメーションのスキンクラスター
	std::shared_ptr<SkinningStatus> mNextSkincluster;
	// モーション遷移時の時間(0.0f ~ 1.0f の間で操作)
	float mMotionBrendingTime;
	// モーションブレンドの切替速度(早 1.0f ~ n 遅い)
	float mMotionBrendingInterval;
	// モーションブレンド中で在るかのフラグ
	bool mIsMotionBrending;
	// アニメーションの再生速度
	float mAnimationPlaySpeed = 1.0f;
	
	// 切り替え待ちのスキンクラスターの可変行列
	std::vector<std::shared_ptr<SkinningStatus>> mWaitingSkinClusters;

};
