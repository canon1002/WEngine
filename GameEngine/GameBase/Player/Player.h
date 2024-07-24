#pragma once
#include "GameEngine/Object/3d/Object3d.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/GameBase/Reticle/Reticle3D.h"
#include "GameEngine/GameBase/Player/Arrow.h"

// 前方宣言
class CollisionManager;

// 振る舞い
enum class Behavior {
	kRoot,   // 通常状態
	kAttack, // 攻撃中
	kJump,	 // ジャンプ中
	kCharge, // 溜め動作中

};

class Player {

public: // -- 公開 メンバ関数 -- //

	Player() = default;
	~Player();

	// 初期化
	void Init();
	// 更新
	void Update();
	// 描画
	void Draw();
	// デバッグUI表示
	void DrawGUI();
	void ColliderDraw();

	Object3d* GetObject3D() { return mObject.get(); }
	Model* GetModel() { return mObject->mModel; }
	Collider* GetCollider() { return mObject->mCollider; }

	Vector3 GetWorldPos() { return mObject->GetWorldTransform()->translation; }
	Reticle3D* GetReticle3D() { return mReticle.get(); }

	void SetColliderListForArrow(CollisionManager* cManager);

	// 攻撃処理
	void Attack();
	// 溜め処理
	void Charge();
	// 溜め->攻撃 移行処理
	void ChengeChageToAttack();

	// アロー生成関数
	Arrow* CreateArrow(Vector3 startPos,Vector3 endPos);

private: // -- 非公開 メンバ変数 -- //

	// 入力を取得
	InputManager* mInput;

	// オブジェクトクラス
	std::unique_ptr<Object3d> mObject;

	// 自機の振る舞い
	Behavior mBehavior;

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

	// 3D照準クラス
	std::unique_ptr<Reticle3D> mReticle;

	// ため動作継続時間
	int32_t mChargeCount;

	// アロークラス
	std::list<Arrow*> mArrows;
	 
};

