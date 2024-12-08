#pragma once
#include "GameEngine/Object/3d/Object3d.h"
#include "App/Status/StatusManager.h"

// 前方宣言
class CollisionManager;

// キャラクター基底クラス
class Actor {

public: // -- 公開 メンバ関数 -- //

	/// <summary>
	/// オブジェクト情報の取得
	/// </summary>
	Object3d* GetObject3D() { return mObject.get(); }
	
	/// <summary>
	/// モデル情報の取得
	/// </summary>
	/// <returns></returns>
	Model* GetModel() { return mObject->mModel; }
	
	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPos() { return mObject->GetWorldTransform()->translation; }

	/// <summary>
	/// コライダーのセット
	/// </summary>
	/// <param name="cManager"></param>
	void SetCollider(CollisionManager* cManager);

	
	// -- アクセッサ -- //


	/// <summary>
	/// ヒットストップの時間を設定
	/// </summary>
	/// <param name="duration">ヒットストップする時間</param>
	void SetHitStopDuration(float duration) { mHitStopDuration = duration; }

	/// <summary>
	/// ヒットストップの時間を取得
	/// </summary>
	/// <returns></returns>
	float GetHitStopDuration() { return mHitStopDuration; }

protected: // -- 限定公開 メンバ関数 -- //

	// -- 初期化関連 -- //

	/// <summary>
	///  部位コライダー生成
	/// </summary>
	/// <param name="name">部位の名称</param>
	/// <param name="radius">コライダーの半径</param>
	void CreateBodyPartCollider(std::string name, float radius, uint32_t atribute, uint32_t mask);


	// -- 更新処理関連 -- //


	/// <summary>
	/// オブジェクト更新処理
	/// </summary>
	//void UpdateObject();

	/// <summary>
	/// 部位コライダーやワールド座標の更新
	/// </summary>
	void UpdateBodyMatrix();



protected: // -- 限定公開 メンバ変数 -- //

	// オブジェクトクラス
	std::unique_ptr<Object3d> mObject;
	// 身体の各部位のコライダーを管理するマップ
	std::unordered_map<std::string, std::unique_ptr<Collider>> mBodyPartColliders;
	// 身体の各部位のワールド行列を管理するマップ
	std::unordered_map<std::string, std::unique_ptr<Matrix4x4>> mBodyPartWorldMatrix;

	// ヒットストップ時間
	float mHitStopDuration = 0.0f;

};

