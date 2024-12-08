#pragma once
#include "App/Actor/Actor.h"
#include "App/AI/State/IBossState.h"
#include "App/AI/State/VitalityBossState.h"
#include "App/AI/BehaviorTree/IBehavior.h"
#include "App/Enemy/Action/Action.h"
#include "App/Status/StatusManager.h"

// 前方宣言
class Player;

class BossEnemy : public Actor
{
public: // -- 公開 メンバ関数 -- //

	BossEnemy() = default;
	~BossEnemy() = default;

	// 初期化
	void Init();
	// 初期化 -- ビヘイビアツリー --
	void InitBehavior();
	// 初期化 -- 行動クラス --
	void InitActions();
	// 更新
	void Update();
	void UpdateBehaviorTree();
	void UpdateObject();
	// 描画
	void Draw();
	// デバッグUI表示
	void DrawGUI();
	void ColliderDraw();

	Object3d* GetObject3D() { return mObject.get(); }
	// モデルの取得
	Model* GetModel() { return mObject->mModel; }
	Collider* GetCollider() { return mObject->mCollider; }

	void SetAttackCollider(CollisionManager* cManager);

	// プレイヤーのポインタをセットする
	void SetPlayer(Player* player) { pPlayer = player; }

	// ステートの更新処理
	void UpdateState();

#pragma region コマンド・ステート関連

	// 行動制御
	void SetNextAction(const std::string& key);
	// 行動状況取得
	ACT::Condition GetActionCondition(const std::string& key);
	// 行動クラスのポインタの取得
	ACT::IAction* GetActionClass(const std::string& key);

	// 自身の攻撃命中時に呼び出す関数
	void ReciveDamageTolayer(float power);
	// 能力値取得関数
	Status* GetStatus() { return mStatus; }


	// 各行動 
	
	// 攻撃行動
	// 命名規則 (行動分類 + 距離 + 追加効果 ) ※変更する可能性あり
	// (例: Attack(攻撃) + Long(遠距離) + InstantDeath(即死) )

	// 遠距離攻撃
	void AttackLong();
	// 近距離攻撃
	void AttackClose();
	// 刺突攻撃
	void AttackThrust();
	// ダッシュ攻撃
	void AttackDash();
	// ジャンプ攻撃
	void AttackJump();

	// 移動関連行動
	// 命名規則 (行動分類  + 方向or対象 )

	// プレイヤーに接近する
	void MoveToPlayer();
	// プレイヤーから逃げる
	void EscapeToPlayer();
	// 後退
	void BackStep();
	// ジャンプ
	void Jump(float JumpPower);

	inline void MoveForward() { mObject->mWorldTransform->translation.z += mVelocity.z; }
	

	// -- 変更用関数 -- //

	// 移動させる
	void AddTransform(Vector3 velocity) { mObject->mWorldTransform->translation += velocity; }
	// 回転量を任意の値に変更する
	void SetRotation(Vector3 rotation) { mObject->mWorldTransform->rotation = rotation; }



	// -- 取得関数 -- //

	Vector3 GetWorldPos();
	Vector3 GetWorldPosForTarget();
	
	const WorldTransform* GetWorldPositionSword(int32_t count) { return mWorldTransformSword.at(count); }


	// -- ActionNodeの実行条件を設定する関数 -- //

	// 距離が近い場合に実行
	bool InvokeNearDistance();

	// 距離が遠い場合に実行
	bool InvokeFarDistance();

	// シェイク処理
	void SetShake(float duration, float power);

	// シェイク実行処理
	void ShakeUpdate();

#pragma endregion

	// 右手のワールド座標
	Matrix4x4 mRightHandWorldMat;
	//
	Matrix4x4 mRightHandsWorldMat;

	// 武器
	std::unique_ptr<Object3d> mWeapon;

	// 武器の衝突判定
	std::vector<Collider*> mWeaponColliders;

private: // -- 非公開 メンバ変数 -- //

	// プレイヤー(ターゲット)のポインタ
	Player* pPlayer;

	// 移動量
	Vector3 mVelocity;

	// 重力の影響を受けるか
	bool mIsGravity;
	// 残りジャンプ回数
	int32_t mJumpCount;
	// 最大ジャンプ回数
	const int32_t kJumpCountMax = 2;
	// ジャンプ初速
	const float kJumpFirstSpeed = 0.4f;

	// 最大コンボ回数
	const int32_t kComboCountMax = 3;

	// メンバ関数ポインタ
	static void(BossEnemy::*CommandTable[])();

	// ボスのState番号の配列(実行中に増減しないため、長さ固定の配列を宣言)
	std::array<std::unique_ptr<IBossState>, 1> mStateArr;
	int mCurrentStateNo;
	int mPrevStateNo;

	// ビヘイビアツリー
	std::unique_ptr<BT::Sequence> mRoot;
	// ビヘイビアツリーの駆動状態
	BT::NodeStatus mBTStatus;

	// ビヘイビアツリーの再実行カウント
	float mReloadBTCount;

	// 行動マップデータ
	std::map<std::string,ACT::IAction*> mActions;
	// 現在の行動
	ACT::IAction* mActiveAction;

	// 能力値
	Status* mStatus;

	// -- エフェクト関係 -- //

	// 剣先と根本のワールド座標
	std::array<WorldTransform*, 2> mWorldTransformSword;

	// 武器の各パーツの座標
	std::array<Matrix4x4, 5> mWeaponWorldMat;


	// シェイク関連のメンバ変数
	float mShakeDuration;
	float mShakePower;
	float mShakeTime;


};
