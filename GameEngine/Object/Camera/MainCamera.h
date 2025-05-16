#pragma once
#include "GameEngine/Utility/Math/Math.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Utility/Transform/WorldTransform.h"
#include "GameEngine/Object/Camera/CameraCommon.h"
#include "GameEngine/Input/InputManager.h"

class MainCamera:
	public CameraCommon
{
public: // -- public メンバ関数 -- //

	MainCamera() = default;
	~MainCamera() = default;

	// インスタンス取得
	static MainCamera* GetInstance();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;

	void UpdateRotationEasing();

	// フォローカメラ機能 -- フォロー対象設定 -- 
	inline void SetFollowTarget(const WorldTransform* target) { mFollowTarget = target; }
	// フォローカメラ機能 -- フォロー解除 -- 
	inline void EraseFollowTarget() { mFollowTarget = nullptr; }

	// 追跡対象設定
	inline void SetSearchTarget(const WorldTransform* target) { mSearchTarget = target; }
	// 追跡対象解除
	inline void EraseSearchTarget() { mSearchTarget = nullptr; }
	// ターゲットへのカメラ回転の解除
	inline void EndSearchTarget() { mRotaionEasingTime = 1.0f; };

	// カメラを追跡対象の方向へ向ける
	void SetCameraRotarionToSearchTarget();

	// カメラを操作キャラの進行方向に合わせる
	void SetCameraRotationToDirection(const Vector3 direction);


	// フォローカメラ機能 -- 追加平行移動値の設定 -- 
	inline void SetAddTranslation(const Vector3 translation){
		mAddTranslation = translation;
	}

	inline void SetCameraRotateControll(bool flag) {
		mIsControll = flag;
	}

	// オフセットの入力
	void SetOffset(const Vector3& offset);
	// オフセットの算出・取得
	Vector3 GetOffset()const;

private: // -- private メンバ関数 -- //

	// コピーコンストラクタと演算子オーバーロードの禁止
	MainCamera(const MainCamera& obj) = delete;
	MainCamera& operator=(const MainCamera& obj) = delete;
	
private: // -- private メンバ変数 -- //

	// インスタンス
	static MainCamera* instance;
	
	// フォロー対象の座標
	const WorldTransform* mFollowTarget;
	// フォロー対象が一定時間前にいた座標を保持する変数
	Vector3 mInterFollowTarget;
	// フォロー対象からカメラまでの距離
	Vector3 mOffset;

	// 追跡対象の座標
	const WorldTransform* mSearchTarget;
	
	// 遷移後回転量
	Vector3 mEaseBeforeRotation;
	Vector3 mEasedRotation;
	// 回転量の遷移中か
	bool mIsRotationEasing;
	float mRotaionEasingTime = 0.0f;
	
	// 追加平行移動値
	Vector3 mAddTranslation;
	// カメラの回転操作を有効にするか
	bool mIsControll;

	// カメラ回転操作の感度
	float mCameraSensitivity;

	// カメラ回転操作の入力経過時間
	Vector2 mCameraInputCounts;
	const float mCamerainputCountMax = 1.0f;

	// カメラ回転操作の制限
	Vector3 mRotLimitMin;
	Vector3 mRotLimitMax;

};

