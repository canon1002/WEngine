#pragma once
#include "App/Actor/Actor.h"
#include "App/AI/State/IBossState.h"
#include "App/AI/State/VitalityBossState.h"
#include "App/Manager/GameManager.h"

// 前方宣言
class Player;

class BossEnemy : public Actor
{
public: // -- 公開 メンバ関数 -- //

	BossEnemy();
	~BossEnemy();

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
	Model* GetModel() { return mObject->mModel.get(); }
	Collider* GetCollider() { return mObject->mCollider.get(); }
	// ゲームマネージャにコライダーを渡す
	void SetColliderList();

	/// <summary>
	/// 攻撃を当てたことをアクターに伝える
	/// </summary>
	virtual void Hit()override {
		mActiveAction.lock()->Hit();
	}

	// プレイヤーのポインタをセットする
	void SetTarget(Actor* target) { mTarget = target; }

	// ステートの更新処理
	void UpdateState();

#pragma region コマンド・ステート関連


	// 自身が攻撃中であるか確認する関数
	bool GetIsOperating()const;

	// ワールド座標を取得
	const WorldTransform* GetWorldPositionSword(int32_t count) { return mWorldTransformSword.at(count).get(); }
	
	// シェイク処理
	void SetShake(float duration, float power);

	// シェイク実行処理
	void ShakeUpdate();

	// リアクション処理(のけぞり)
	void SetKnockBackCount(int32_t count);

	// 体の座標を取得する
	Vector3 GetBodyPos();

#pragma endregion

public: // -- 公開 メンバ変数 -- //

	// 右手のワールド座標
	Matrix4x4 mRightHandWorldMat;
	// 武器
	std::unique_ptr<Object3d> mWeapon;
	// 武器の衝突判定
	std::vector<std::shared_ptr<GameCollider>> mWeaponColliders;


private: // -- 非公開 メンバ変数 -- //

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

	

	// メンバ関数ポインタ
	static void(BossEnemy::*CommandTable[])();

	// ボスのState番号の配列(実行中に増減しないため、長さ固定の配列を宣言)
	std::array<std::unique_ptr<IBossState>, 1> mStateArr;
	int mCurrentStateNo;
	int mPrevStateNo;

	// ビヘイビアツリーの駆動状態
	AINode::State mBTStatus;

	// ビヘイビアツリーの再実行カウント
	float mReloadBTCount;
	// ビヘイビアツリーを実行するか
	bool mIsUpdateBehavior = true;

	// -- エフェクト関係 -- //

	// 剣先と根本のワールド座標
	std::array<std::unique_ptr<WorldTransform>, 2> mWorldTransformSword;

	// 武器の各パーツの座標
	std::array<Matrix4x4, 9> mWeaponWorldMat;


	// シェイク関連のメンバ変数
	float mShakeDuration;
	float mShakePower;
	float mShakeTime;


	// 仮ノックバック処理
	int32_t mKnockBackCount;
	const int32_t kKnockBackCountMax = 7;


};
