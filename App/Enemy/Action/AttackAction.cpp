#include "Action.h"
#include "App/Enemy/BossEnemy.h"
#include "GameEngine/Append/Collider/SphereCollider.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Append/Collider/CollisionManager.h"
#include "GameEngine/GameMaster/Framerate.h"

void ACT::AttackClose::Init(BossEnemy* boss)
{
	// ボスのポインタを取得
	mBoss = boss;
	// 初期化する
	mCondition = Condition::IDOL;
	// コライダー表示期間の初期化
	mActiveColliderCount = 0.0f;

}

void ACT::AttackClose::Update()
{
	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {

		// 衝突判定を出す時刻を進める
		mActiveColliderCount += (2.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetBattleSpeed();


		// 各コライダーの衝突判定を確認
		if (!mIsHit && mIsOperating) {
			for (Collider* collider : mBoss->mWeaponColliders) {
				if (collider->GetOnCollisionFlag() == true) {
					mBoss->ReciveDamageTolayer(1.0f);
					mIsHit = true;
					// どれか１つでも命中したらループを抜ける
					break;
				}
			}
		}

		// 終了処理
		if (mBoss->GetObject3D()->mSkinning->GetIsAnimationFinished("Slash")) {
			mCondition = Condition::FINISHED;
		}


	}
}

void ACT::AttackClose::Draw() {

	if (!mIsHit && mIsOperating) {
		// 武器のコライダー 描画
		for (Collider* collider : mBoss->mWeaponColliders) {
			collider->Draw();
		}
	}

}

void ACT::AttackClose::Start()
{
	// パラメータの初期化

	// アニメーションの変更
	mBoss->GetObject3D()->mSkinning->SetNextAnimation("Slash");

	mActiveColliderCount = 0.0f;

	mIsHit = false;
	mIsOperating = true;

	// 実行する
	mCondition = Condition::RUNNING;
}

void ACT::AttackClose::End()
{
	// 行動を終了させる
	mCondition = Condition::FINISHED;

	// アニメーションの変更
	//mBoss->GetObject3D()->mSkinning->SetNextAnimation("Idle");
}

void ACT::AttackClose::Reset()
{
	// 初期化する
	mCondition = Condition::IDOL;
}

void ACT::AttackClose::SetCollider(CollisionManager* cManager)
{
	// 未ヒット時にのみコライダーセット
	for (Collider* collider : mBoss->mWeaponColliders) {
		cManager->SetCollider(collider);
	}
}



void ACT::AttackThrust::Init(BossEnemy* boss)
{
	// ボスのポインタを取得
	mBoss = boss;
	// 初期化する
	mCondition = Condition::IDOL;

	mActiveColliderCount = 0.0f;
}

void ACT::AttackThrust::Update()
{
	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {

		// 衝突判定を出す時刻を進める
		mActiveColliderCount += (2.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetBattleSpeed();

		// 各コライダーの衝突判定を確認
		if (!mIsHit && mIsOperating) {
			for (Collider* collider : mBoss->mWeaponColliders) {
				if (collider->GetOnCollisionFlag() == true) {
					mBoss->ReciveDamageTolayer(1.0f);
					mIsHit = true;
					// どれか１つでも命中したらループを抜ける
					break;
				}
			}
		}

		// 終了処理
		if (mBoss->GetObject3D()->mSkinning->GetIsAnimationFinished("Thrust")) {
			mCondition = Condition::FINISHED;
		}


	}
}

void ACT::AttackThrust::Draw() {

	if (!mIsHit && mIsOperating) {
		// 武器のコライダー 描画
		for (Collider* collider : mBoss->mWeaponColliders) {
			collider->Draw();
		}
	}

}

void ACT::AttackThrust::Start()
{
	// パラメータの初期化

	// アニメーションの変更
	mBoss->GetObject3D()->mSkinning->SetNextAnimation("Thrust");
	mActiveColliderCount = 0.0f;

	mIsHit = false;
	mIsOperating = true;

	// プレイヤーの方を向く

	// ここから回転処理
	// 方向の設定
	Vector3 direction = Normalize(mBoss->GetWorldPosForTarget() - mBoss->GetWorldPos());
	direction.y = 0.0f;
	const float PI = 3.14f;
	float rotateY = std::atan2f(direction.x, direction.z);
	rotateY = std::fmodf(rotateY, 2.0f * PI);
	if (rotateY > PI) {
		rotateY -= 2.0f * PI;
	}
	if (rotateY < -PI) {
		rotateY += 2.0f * PI;
	}

	// 計算結果をBossクラスに渡す
	mBoss->SetRotation(Vector3(0.0f, rotateY, 0.0f));

	// 刺突のときは武器を 右手 にペアレントする
	//mBoss->mWeapon->mWorldTransform->SetParent(mBoss->mRightHandsWorldMat);

	// 実行する
	mCondition = Condition::RUNNING;
}

void ACT::AttackThrust::End() 
{
	// 行動を終了させる
	mCondition = Condition::FINISHED;

	// アニメーションの変更
	//mBoss->GetObject3D()->mSkinning->SetNextAnimation("Idle");

	// 終わったら戻す
	//mBoss->mWeapon->mWorldTransform->SetParent(mBoss->mRightHandWorldMat);

}

void ACT::AttackThrust::Reset()
{
	// 初期化する
	mCondition = Condition::IDOL;
}

void ACT::AttackThrust::SetCollider(CollisionManager* cManager)
{
	// 未ヒット時にのみコライダーセット
	for (Collider* collider : mBoss->mWeaponColliders) {
		cManager->SetCollider(collider);
	}

}

