#pragma once
#include "App/Actor/Actor.h"
#include "GameEngine/Input/InputManager.h"
#include "App/Reticle/Reticle3D.h"
#include "App/Manager/GameManager.h"

// 前方宣言
class BossEnemy;

// 振る舞い
enum class Behavior {
	kRoot,  // 通常状態
	kMove,  // 移動状態
	kDash,	// ダッシュ状態
	kAttack,	// 攻撃中
	kJump,	// ジャンプ中
	kCharge,	// 溜め動作中
	kChargeAttack, // 溜め攻撃
	kAvoid,  // 回避行動
	kGuard,  // 防御行動
};

// ワーク構造体
// 各行動にて共通で使用しない変数をまとめた構造体
// 各行動の調整がある程度できたらStateパターンへ移行する

struct WorkDash{

};

// 攻撃用定数
struct ConstAttack {
	float offence;			// 威力
	float motionTimeMax;	// 終了時間
	float actionSpeed;		// 実行速度
	float inputWaitTime;	// 入力受付時間
	float recoveryTime;		// 硬直時間
};

struct AttackStatus {

	std::unique_ptr<Object3d> sword; // オブジェクト
	bool isUp;			// 数値が上昇しているか
	bool isDown;		// 数値が減少しているか
	bool isOperating;	// 動作中であるか(攻撃判定フラグとしても扱う)
	bool isHit;			// 攻撃が命中したか
	float motionTime;	// 動作の時間
	Vector3 pos;		// 座標
	Vector3 normalRot;	// 通常時の回転
	Vector3 endRot;		// 回転の最終値
	Matrix4x4 weaponParentMat; // 親の行列
	std::array<Matrix4x4, 5> swordWorldMat; // 武器のワールド座標
	std::vector<std::shared_ptr<GameCollider>> swordColliders;// 武器の衝突判定

	int32_t comboCount; // 現在のコンボが何段目か
	int32_t inComboPhase; // 現在の攻撃モーションはどの段階か
	float inputWaitingTime; // 入力待ち段階
	bool isComboRequest; // 入力を受けたか
};


struct AvoidStatus {
	// 回避行動リクエスト
	bool mIsAvoidRequest;
	// 回避中か
	bool mIsAvoid;
	// 回避時の無敵状態であるか
	bool mIsAvoidInvincible;
	// 回避距離
	float mAvoidRange;
	// 回避速度
	float mAvoidSpeed;
	// 回避経過時間( 0.0f ~ 1.0f )
	float mAvoidTime;
	// 回避開始地点
	Vector3 mAvoidMoveStartPos;
	// 回避終了地点
	Vector3 mAvoidMoveEndPos;
};

class Player : public Actor {

public: // -- 公開 メンバ関数 -- //

	Player();
	~Player();

	// 初期化
	void Init();

	
	// 全体更新
	void Update();
	// 座標/アニメーション更新
	void UpdateObject();

	// 描画
	void Draw();
	// デバッグUI表示
	void DrawGUI();
	void ColliderDraw();

	Object3d* GetObject3D() { return mObject.get(); }
	Model* GetModel() { return mObject->mModel.get(); }
	Collider* GetCollider() { return mObject->mCollider.get(); }
	const Matrix4x4& GetSwordBoneMatrix(int32_t count) { return mAttackStatus.swordWorldMat.at(count); }
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
	Behavior GetBehavior()const { return mBehavior; }
	// 自身が攻撃中であるか確認する
	bool GetIsOperating() const;
	// ロックオンの有無を取得する
	bool GetIsLockOn()const { return mIsLockOnToTarget; }

	// 回避
	void Avoid();
	// 攻撃処理
	void Attack();
	// 射撃処理
	void Shot();
	// 移動処理
	void Move();
	// 落下処理
	void Fall();

	// 入力状況をDirectionに保存する
	void InputDirection();

	// ボスクラスのポインタをセットする
	void SetBoss(BossEnemy* boss) { mBoss = boss; }

	/// <summary>
	/// 攻撃を当てたことをアクターに伝える
	/// </summary>
	virtual void Hit()override {
		mAttackStatus.isHit = true;
	}

	// ボスクラスのポインタ
	BossEnemy* mBoss;

private: // -- 非公開 メンバ関数 -- //

	// 保存処理
	virtual void Save() override;
	// 読み込み処理
	virtual void Load() override;
	

public: // -- 公開 メンバ変数 & 定数 -- //

	// 最大コンボ回数
	static const int32_t kComboCountMax = 3;
	// コンボ定数表
	static const std::array<ConstAttack, kComboCountMax> kConstAttacks;

private: // -- 非公開 メンバ変数 -- //
	
	// 自機の振る舞い
	Behavior mBehavior;

	// 移動量
	Vector3 mVelocity;


	// 方向入力時間
	float mDirectionInputCount;
	// 

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

	// 攻撃関連パラメータ
	AttackStatus mAttackStatus;
	// 回避関連パラメータ
	AvoidStatus mAvoidStatus;

	// -- カメラ操作関係 -- //

	// カメラ回転ロックの有無
	bool mIsCameraRotateLock;
	// ターゲットロックオンの有無
	bool mIsLockOnToTarget;

	// -- エフェクト関係 -- //

	// 剣先と根本のワールド座標
	array<unique_ptr<WorldTransform>,2> mWorldTransformSword;

};

