#pragma once
#include "App/Actor/Actor.h"
#include "GameEngine/Input/InputManager.h"
#include "App/Reticle/Reticle3D.h"
#include "App/Manager/GameManager.h"

// 前方宣言
class BossEnemy;

class Player : public Actor {

public: // -- 公開 メンバ関数 -- //

	Player();
	~Player();

	// 初期化
	void Init();
	void InitObject();	// オブジェクト・モデル関係
	void InitWorks();	// 各行動

	// 更新
	void Update();
	void UpdateObject();	// 座標/アニメーション更新

	// 描画
	void Draw();
	// デバッグUI表示
	void DrawGUI();
	void ColliderDraw();



	Object3d* GetObject3D() { return mObject.get(); }
	Model* GetModel() { return mObject->mModel.get(); }
	Collider* GetCollider() { return mObject->mCollider.get(); }
	const Matrix4x4& GetSwordBoneMatrix(int32_t count) { return swordWorldMat.at(count); }
	const WorldTransform* GetWorldPositionSword(int32_t count) { return mWorldTransformSword.at(count).get(); }

	// ワールド座標の取得
	Vector3 GetWorldPos() { return mObject->GetWorldTransform()->translation; }
	// 移動量の取得
	Vector3 GetVelocity()const { return mVelocity; }
	// 移動量の設定
	void SetVelocity(const Vector3& vel){ mVelocity = vel; }


	Reticle3D* GetReticle3D() { return mReticle.get(); }

	// ゲームマネージャにコライダーを追加する
	void SetColliderList();

	// 自身のふるまい・状況を取得する
	Behavior GetBehavior()const { return mWorks->mBehavior; }
	// 自身が攻撃中であるか確認する
	bool GetIsOperating() const;
	// ロックオンの有無を取得する
	bool GetIsLockOn()const { return mIsLockOnToTarget; }

	// ボスクラスのポインタをセットする
	void SetBoss(BossEnemy* boss) { mBoss = boss; }

	/// <summary>
	/// 攻撃を当てたことをアクターに伝える
	/// </summary>
	virtual void Hit()override {
		mWorks->mWorkAttack.isHit = true;
	}

	// ボスクラスのポインタ
	BossEnemy* mBoss;

	// ワーク外処理
	void Shot();	// 射撃処理
	void Fall();	// 落下処理


private: // -- 非公開 メンバ関数 -- //

	// -- 更新関数 -- //

	// 入力部分処理
	void Input();
	// 入力状況をDirectionに保存する
	void InputDirection();

	// レティクル・ロックオン関係処理
	void LockOn();

	// 各ワークごとの処理をメンバ関数ポインタで管理する
	void (Player::* mWorkFunc)();
	void Move();	// 移動処理
	void Attack();	// 攻撃処理
	void Avoid();	// 回避処理
	void Guard();	// 防御処理
	void JustAction();	// ジャストアクション


public: // -- 公開 メンバ変数 & 定数 -- //

	// 最大コンボ回数
	static const int32_t kComboCountMax = 3;
	// コンボ定数表
	std::array<ConstAttack, kComboCountMax> kConstAttacks;

private: // -- 非公開 メンバ変数 -- //
	
	// 移動量
	Vector3 mVelocity;
	// 方向入力時間
	float mDirectionInputCount;
	// 重力の影響を受けるか
	bool mIsGravity;
	// 残りジャンプ回数
	int32_t mJumpCount;
	// 最大ジャンプ回数
	const int32_t kJumpCountMax = 2;
	// ジャンプ初速
	const float kJumpFirstSpeed = 0.4f;

	// カメラ回転量保持
	Vector3 mCameraPreDir = { 0.0f,0.0f,0.0f };

	// 3D照準クラス
	std::unique_ptr<Reticle3D> mReticle;
	// ため動作継続時間
	int32_t mChargeCount;
	// 狙い撃ちの構えをしているか
	bool mIsAimMode;


	// -- カメラ操作関係 -- //

	// カメラ回転ロックの有無
	bool mIsCameraRotateLock;
	// ターゲットロックオンの有無
	bool mIsLockOnToTarget;


	// -- 武器 -- //

	std::unique_ptr<Object3d> sword; // 剣のオブジェクト
	Matrix4x4 weaponParentMat; // 親の行列
	std::array<Matrix4x4, 5> swordWorldMat; // 武器のワールド座標
	std::vector<std::shared_ptr<GameCollider>> swordColliders;// 武器の衝突判定

	// 剣先と根本のワールド座標
	array<unique_ptr<WorldTransform>,2> mWorldTransformSword;


};

