#pragma once
#include "GameEngine/Object/3d/Object3d.h"
#include "App/Status/StatusManager.h"
#include "App/Enemy/Action/Action.h"

// 前方宣言
class CollisionManager;

// キャラクター基底クラス
class Actor {

public: // -- 公開 メンバ関数 -- //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Actor();
	Actor(const std::string& name);

	/// <summary>
	/// 仮想デストラクタ
	/// </summary>
	virtual ~Actor() {}

	/// <summary>
	/// オブジェクト情報の取得
	/// </summary>
	Object3d* GetObject3D() { return mObject.get(); }
	
	/// <summary>
	/// モデル情報の取得
	/// </summary>
	/// <returns></returns>
	Model* GetModel() { return mObject->mModel.get(); }
	
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

#pragma region アクション関連

	/// <summary>
	/// 行動指定関数
	/// </summary>
	/// <param name="key">行動内容(クラス名)</param>
	virtual void SetAction(const std::string& key);

	/// <summary>
	/// 行動状況取得
	/// </summary>
	/// <param name="key">行動内容(クラス名)</param>
	ACT::Condition GetActionCondition(const std::string& key);

	/// <summary>
	/// 行動クラスのポインタの取得
	/// </summary>
	/// <param name="key">行動内容(クラス名)</param>
	ACT::Action* GetActionClass(const std::string& key);

	/// <summary>
	/// 向かせたい向きを指定する
	/// </summary>
	/// <param name="dir">方向</param>
	void InputDirection(const Vector3& dir);

	/// <summary>
	/// Directionを徐々に入力方向に合わせる
	/// </summary>
	void AdJustDirection();

	void ReceiveDamage();

#pragma endregion

#pragma region  アクション条件・簡易行動

	// 移動させる
	void AddTransform(Vector3 velocity) { mObject->mWorldTransform->translation += velocity; }
	// 回転量を任意の値に変更する
	void SetRotation(Vector3 rotation) { mObject->mWorldTransform->rotation = rotation; }

	// ターゲットの座標を取得
	Vector3 GetWorldPosForTarget();

	// 距離が近い場合に実行
	bool InvokeNearDistance();
	// 距離が近い場合に実行(距離設定可能)
	bool IsNearDistance(float range);

	// 距離が遠い場合に実行
	bool InvokeFarDistance();
	// 距離が遠い場合に実行(距離設定可能)
	bool IsFarDistance(float range);

#pragma endregion 


	// -- アクセッサ -- //


	// 能力値取得関数
	std::shared_ptr<Status> GetStatus() { return mStatus; }


	/// <summary>
	/// アクターの名称を取得する
	/// </summary>
	std::string GetName() { return mName; }


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

	// 保存処理
	virtual void Save();
	// 読み込み処理
	virtual void Load();

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

	// アクターの名称
	std::string mName;

	// オブジェクトクラス
	std::unique_ptr<Object3d> mObject;
	// 身体の各部位のコライダーを管理するマップ
	std::unordered_map<std::string, std::shared_ptr<Collider>> mBodyPartColliders;
	// 身体の各部位のワールド行列を管理するマップ
	std::unordered_map<std::string, std::shared_ptr<Matrix4x4>> mBodyPartWorldMatrix;

	// ヒットストップ時間
	float mHitStopDuration = 0.0f;

	// 能力値
	std::shared_ptr<Status> mStatus;

	// 行動マップデータ
	std::map<string, std::shared_ptr<ACT::Action>> mActions;
	// 現在の行動
	std::weak_ptr<ACT::Action> mActiveAction;

	// ターゲット対象のポインタ
	Actor* mTarget;

	// アクターの向き
	Vector3 mDirection;
	// 入力方向
	Vector3 mDirectionForInput;

};

