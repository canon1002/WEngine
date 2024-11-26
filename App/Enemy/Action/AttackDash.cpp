#include "AttackDash.h"
#include "App/Enemy/BossEnemy.h"
#include "GameEngine/Append/Collider/SphereCollider.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Append/Collider/CollisionManager.h"
#include "GameEngine/GameMaster/Framerate.h"

void ACT::AttackDash::Init(BossEnemy* boss)
{
	// ボスのポインタを取得
	mBoss = boss;
	// 初期化する
	mCondition = Condition::IDOL;


	/*mColliders.push_back(new SphereCollider(mWeapon->mWorldTransform, 0.25f));
	mColliders.push_back(new SphereCollider(mWeapon->mWorldTransform, 0.25f));
	mColliders.push_back(new SphereCollider(mWeapon->mWorldTransform, 0.25f));*/

	mActiveColliderCount = 0.0f;



}

void ACT::AttackDash::Update()
{
	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {

		// 衝突判定を出す時刻を進める
		mActiveColliderCount += (2.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetBattleSpeed();


		// 各コライダーの衝突判定を確認
		if (kActiveColliderCount.x < mActiveColliderCount &&
			mActiveColliderCount < kActiveColliderCount.y) {
			for (Collider* collider : mBoss->mWeaponColliders) {
				if (collider->GetOnCollisionFlag() == true) {
					mBoss->ReciveDamageTolayer(1.0f);
					mActiveColliderCount += 2.0f;
					// どれか１つでも命中したらループを抜ける
					break;
				}
			}
		}

		// 終了処理
		if (mBoss->GetObject3D()->mSkinning->IsAnimationFinished() == true) {
			mCondition = Condition::FINISHED;
		}

	}
}

void ACT::AttackDash::Draw() {

	if (kActiveColliderCount.x < mActiveColliderCount &&
		mActiveColliderCount < kActiveColliderCount.y) {

		//mBoss->mWeapon->mCollider->Draw();

		// 武器のコライダー 描画
		for (Collider* collider : mBoss->mWeaponColliders) {
			collider->Draw();
		}
	}

}

void ACT::AttackDash::Start()
{
	// パラメータの初期化

	// アニメーションの変更
	mBoss->GetObject3D()->mSkinning->SetNextAnimation("SlashDash");

	mActiveColliderCount = 0.0f;

	// 実行する
	mCondition = Condition::RUNNING;
}

void ACT::AttackDash::End()
{
	// 行動を終了させる
	mCondition = Condition::FINISHED;

	// アニメーションの変更
	mBoss->GetObject3D()->mSkinning->SetNextAnimation("Idle");
}

void ACT::AttackDash::Reset()
{
	// 初期化する
	mCondition = Condition::IDOL;
}

void ACT::AttackDash::SetCollider(CollisionManager* cManager)
{
	if (kActiveColliderCount.x < mActiveColliderCount &&
		mActiveColliderCount < kActiveColliderCount.y) {

		// 未ヒット時にのみコライダーセット
		for (Collider* collider : mBoss->mWeaponColliders) {
			cManager->SetCollider(collider);
		}
	}
}