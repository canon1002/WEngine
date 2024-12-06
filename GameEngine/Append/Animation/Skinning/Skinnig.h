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

/// <summary>
/// スキニングアニメーションクラス
/// </summary>
class Skinning
{
public: // -- 公開 メンバ関数 -- //
	
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="directorypath">ディレクトリパス</param>
	/// <param name="filepath">ファイルパス</param>
	/// <param name="modelData">モデルデータ</param>
	void Init(const std::string& directorypath, const std::string& filepath, ModelData modelData);

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
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// ImGuiの表示
	/// </summary>
	void DrawGui();


	//
	// -- アクセッサ (設定関数) -- //
	//


	/// <summary>
	/// アニメーションの再生速度を設定
	/// </summary>
	/// <param name="speed"></param>
	void SetAnimationPlaySpeed(float speed) { mAnimationPlaySpeed = speed; }

	/// <summary>
	/// 一時停止する
	/// </summary>
	inline void Pause() { mNowSkincluster->isPause = true; }

	/// <summary>
	/// アニメーションを再生する(一時停止やアニメーション停止も解除する)
	/// </summary>
	inline void Play() { mNowSkincluster->isPause = false; }

	/// <summary>
	/// アニメーションのループ設定(trueでループ)
	/// </summary>
	/// <param name="isLoop">ループさせるか</param>
	void SetLoopMode(bool isLoop) { mNowSkincluster->isLoop = isLoop; }

	/// <summary>
	/// アニメーションを行わないようにする
	/// </summary>
	void IsInactive() { mNowSkincluster->isActive = false; }

	/// <summary>
	/// 次に再生するアニメーションを設定する
	/// </summary>
	/// <param name="name">アニメーション名</param>
	void SetNextAnimation(std::string name);

	/// <summary>
	/// モーションブレンドを開始する
	/// <para> 次回のアニメーションが設定されていない場合は停止する </para>
	/// </summary>
	void StartMotionBrend();

	/// <summary>
	/// モーションブレンドを終了する
	/// </summary>
	void EndMotionBrend();

	/// <summary>
	/// モーションブレンドの切替速度を変更する
	/// </summary>
	/// <param name="interval"> モーション切替速度 1.0f ~ n </param>
	void SetMotionBlendingInterval(float interval) { mMotionBrendingInterval = interval; }



	//
	// -- アクセッサ (取得関数) -- //
	//


	/// <summary>
	// 現在再生しているアニメーションのスキンクラスターを取得
	/// </summary>
	/// <returns> スキンクラスター及びアニメーション情報</returns>
	std::shared_ptr<SkinningStatus> GetNowSkinCluster() { return mNowSkincluster; }

	/// <summary>
	/// 指定した名前のスキンクラスターを取得
	/// </summary>
	/// <param name="name">アニメーション名</param>
	/// <returns>スキンクラスター及びアニメーション情報</returns>
	std::shared_ptr <SkinningStatus> GetSkinCluster(std::string name) { return mSkinClusterMap.at(name); }
	
	/// <summary>
	/// ボーン情報を取得する
	/// </summary>
	/// <returns> ボーンの情報 </returns>
	Skeleton GetSkeleton() { return mSkeleton;}

	/// <summary>
	/// アニメーションが設定されているか取得(=ボーンのみ使用かの判別)
	/// </summary>
	/// <returns>ボーンのみ使用している場合は"false"を、それ以外は"true"を返す</returns>
	bool GetIsActive() { return mNowSkincluster->isActive; }

	// <summary>
	/// 現在再生中のアニメーションが終了しているかを取得
	/// </summary>
	/// <returns> 終了していれば"true"を、それ以外は"false"を返す </returns>
	inline bool GetIsNowAnimationFinished()const {
		if (mNowSkincluster->animationTime >= (mNowSkincluster->animation.duration)) {
			return true;
		}
		return false;
	}
	
	/// <summary>
	/// 指定した名称のアニメーションが終了しているかを取得
	/// </summary>
	/// <returns> 終了していれば"true"を、それ以外は"false"を返す </returns>
	inline bool GetIsAnimationFinished(std::string name) {
		// アニメーションが存在するか確認する
		if (mSkinClusterMap.find(name) == mSkinClusterMap.end()) {
			return false;
		}

		// 再生時間と全体の尺を比較する
		if (mSkinClusterMap.at(name)->animationTime >= mSkinClusterMap.at(name)->animation.duration) {
			return true;
		}
		return false;
	}

	// 指定したアニメーションがどれほど再生されているのか割合で取得する
	inline float GetAnimationTimeRate(std::string name) {
		// アニメーションが存在するか確認する
		if (mSkinClusterMap.find(name) == mSkinClusterMap.end()) {
			// 存在しない場合は0.0fを返す
			return 0.0f;
		}
		// アニメーションの再生時間を全体の尺で割合を取得する
		return mSkinClusterMap.at(name)->animationTime / mSkinClusterMap.at(name)->animation.duration;
	}


	/// <summary>
	/// モーションブレンドの移行時間を取得
	/// </summary>
	float GetMotionBlendingTime()const { return mMotionBrendingTime; }

	/// <summary>
	/// 現在再生しているアニメーションの全体の尺を取得する
	/// </summary>
	/// <returns></returns>
	float GetDurationTime()const { return mNowSkincluster->animation.duration; }

	/// <summary>
	/// アニメーション全体の尺を取得する(登録済みのアニメーションのMAPから指定する)
	/// </summary>
	/// <param name="name"> アニメーション名 </param>
	/// <returns>アニメーション全体の尺</returns>
	float GetDurationTime(std::string name)const { return mSkinClusterMap.at(name)->animation.duration; }

	/// <summary>
	/// モーションブレンドを行っているか取得
	/// </summary>
	/// <returns>モーションブレンド中であれば"true"を、それ以外はfalseを返す</returns>
	bool GetIsMotionbrending()const { return mIsMotionBrending; }

	/// <summary>
	/// 現行のアニメーションが指定した名前であるか確認
	/// </summary>
	/// <param name="name"> アニメーション名 </param>
	/// <returns> 名称が一致していれば"true"を、それ以外は"false"を返す </returns>
	bool GetIsActiveAnimation(std::string name);

	/// <summary>
	/// モーションブレンドを待機しているアニメーション配列を取得
	/// </summary>
	/// <returns>アニメーション待機配列を返す</returns>
	std::vector<std::shared_ptr<SkinningStatus>> GetWaitingSkinClusters() { return mWaitingSkinClusters; }

	/// <summary>
	/// 待機しているアニメーション配列に指定したアニメーションがあるか検索
	/// </summary>
	/// <param name="name"> アニメーション名 </param>
	/// <returns> 配列に存在する場合はtrueを返す。存在しなければ"false"を返す </returns>
	bool SearchToWaitingSkinCluster(std::string name);

private: // -- 非公開 メンバ関数 -- //

	/// <summary>
	/// Animationを適用する
	/// </summary>
	void ApplyAniation();

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
