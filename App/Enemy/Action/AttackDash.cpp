#include "AttackDash.h"
#include "App/Actor/Actor.h"
#include "GameEngine/Append/Collider/SphereCollider.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Append/Collider/CollisionManager.h"
#include "GameEngine/GameMaster/Framerate.h"

void ACT::AttackDash::Init(Actor* boss)
{
	// ボスのポインタを取得
	mActor = boss;
	// 初期化する
	mCondition = Condition::IDOL;

	//
	mActiveColliderCount = 0.0f;

}

void ACT::AttackDash::Update()
{
	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {

		// 衝突判定を出す時刻を進める
		mActiveColliderCount += (2.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetBattleSpeed();


		//// 各コライダーの衝突判定を確認
		//if (!mIsHit && mIsOperating) {
		//	for (Collider* collider : mActor->mWeaponColliders) {
		//		if (collider->GetOnCollisionFlag() == true) {
		//			mActor->ReciveDamageTolayer(1.0f);
		//			mIsHit = true;
		//			// どれか１つでも命中したらループを抜ける
		//			break;
		//		}
		//	}
		//}

		// 終了処理
		if (mActor->GetObject3D()->mSkinning->GetIsAnimationFinished("SlashDash")) {
			mCondition = Condition::FINISHED;
		}

	}
}

void ACT::AttackDash::Draw() {

}

void ACT::AttackDash::Start()
{
	// パラメータの初期化

	// アニメーションの変更
	mActor->GetObject3D()->mSkinning->SetNextAnimation("SlashDash");

	mIsHit = false;
	mIsOperating = true;
	mActiveColliderCount = 0.0f;

	// 実行する
	mCondition = Condition::RUNNING;
}

void ACT::AttackDash::End()
{
	// 行動を終了させる
	mCondition = Condition::FINISHED;

	// アニメーションの変更
	//mActor->GetObject3D()->mSkinning->SetNextAnimation("Idle");
}

void ACT::AttackDash::Reset()
{
	// 初期化する
	mCondition = Condition::IDOL;
}

void ACT::AttackDash::Save()
{
}

void ACT::AttackDash::Load()
{
}

void ACT::AttackDash::DrawGui()
{
}
