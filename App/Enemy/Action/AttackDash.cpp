#include "AttackDash.h"
#include "App/Actor/Actor.h"
#include "GameEngine/Append/Collider/SphereCollider.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Append/Collider/CollisionManager.h"
#include "GameEngine/GameMaster/Framerate.h"
#include "App/BlackBoard.h"

void ACT::AttackDash::Init(Actor* boss)
{
	// ボスのポインタを取得
	mActor = boss;
	// 初期化する
	mCondition = Condition::IDOL;
	// パラメータ初期化
	Reset();

}

void ACT::AttackDash::Update()
{
	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {

		// 衝突判定を出す時刻を進める
		mColliderCount += BlackBoard::GetBattleFPS();

		// 衝突判定の受付時間内であれば衝突フラグをtrueにする
		if (!mIsHit && InRange(mColliderCount, mActiveColliderCount)) {
			mIsOperating = true;
		}
		else {
			mIsOperating = false;
		}

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
	// パラメータ初期化
	Reset();
	// アニメーションの変更
	mActor->GetObject3D()->mSkinning->SetNextAnimation("SlashDash");

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
	mIsHit = false;
	mIsOperating = false;
	// 衝突判定発生時間を設定
	mColliderCount = 0.0f;
	mActiveColliderCount = BlackBoard::GetGlobalVariables()->GetVector2Value("AttackDash", "Duration");
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
