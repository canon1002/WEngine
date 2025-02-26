#include "Action.h"
#include "App/Actor/Actor.h"
#include "GameEngine/Append/Collider/SphereCollider.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Append/Collider/CollisionManager.h"
#include "GameEngine/GameMaster/Framerate.h"
#include "App/BlackBoard.h"

void ACT::AttackClose::Init(Actor* actor)
{
	// ボスのポインタを取得
	mActor = actor;
	// 初期化する
	Reset();

}

void ACT::AttackClose::Update()
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
		if (mActor->GetObject3D()->mSkinning->GetIsAnimationFinished("Slash")) {
			mCondition = Condition::FINISHED;
		}


	}
}

void ACT::AttackClose::Draw() {

	

}

void ACT::AttackClose::Start()
{
	// パラメータの初期化
	Reset();
	// アニメーションの変更
	mActor->GetObject3D()->mSkinning->SetNextAnimation("Slash");
	// 実行する
	mCondition = Condition::RUNNING;
}

void ACT::AttackClose::End()
{
	// 行動を終了させる
	mCondition = Condition::FINISHED;
}

void ACT::AttackClose::Reset()
{
	// 初期化する
	mCondition = Condition::IDOL;
	mIsHit = false;
	mIsOperating = false;
	// 衝突判定発生時間を設定
	mColliderCount = 0.0f;
	mActiveColliderCount = BlackBoard::GetGlobalVariables()->GetVector2Value("AttackClose", "Duration");
}

void ACT::AttackClose::Save()
{
}

void ACT::AttackClose::Load()
{
}

void ACT::AttackClose::DrawGui()
{
}

void ACT::AttackThrust::Init(Actor* actor)
{
	// ボスのポインタを取得
	mActor = actor;
	// 初期化する
	mCondition = Condition::IDOL;

	mColliderCount = 0.0f;
}

void ACT::AttackThrust::Update()
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
		if (mActor->GetObject3D()->mSkinning->GetIsAnimationFinished("Thrust")) {
			mCondition = Condition::FINISHED;
		}


	}
}

void ACT::AttackThrust::Draw()
{
}


void ACT::AttackThrust::Start()
{
	// パラメータ初期化
	Reset();
	// アニメーションの変更
	mActor->GetObject3D()->mSkinning->SetNextAnimation("Thrust");
	// プレイヤーの方を向く
	mActor->InputDirection(Normalize(mActor->GetWorldPosForTarget() - mActor->GetWorldPos()));
	
	// 実行する
	mCondition = Condition::RUNNING;
}

void ACT::AttackThrust::End() 
{
	// 行動を終了させる
	mCondition = Condition::FINISHED;
}

void ACT::AttackThrust::Reset()
{
	// 初期化する
	mCondition = Condition::IDOL;
	mIsHit = false;
	mIsOperating = false;
	// 衝突判定発生時間を設定
	mColliderCount = 0.0f;
	mActiveColliderCount = BlackBoard::GetGlobalVariables()->GetVector2Value("AttackDash", "Duration");
}

void ACT::AttackThrust::Save()
{
}

void ACT::AttackThrust::Load()
{
}

void ACT::AttackThrust::DrawGui()
{
}


