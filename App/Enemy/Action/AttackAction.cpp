#include "Action.h"
#include "App/Actor/Actor.h"
#include "GameEngine/Append/Collider/SphereCollider.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Append/Collider/CollisionManager.h"
#include "GameEngine/GameMaster/Framerate.h"

void ACT::AttackClose::Init(Actor* actor)
{
	// ボスのポインタを取得
	mActor = actor;
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

	// アニメーションの変更
	mActor->GetObject3D()->mSkinning->SetNextAnimation("Slash");

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
	//mActor->GetObject3D()->mSkinning->SetNextAnimation("Idle");
}

void ACT::AttackClose::Reset()
{
	// 初期化する
	mCondition = Condition::IDOL;
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

	mActiveColliderCount = 0.0f;
}

void ACT::AttackThrust::Update()
{
	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {

		// 衝突判定を出す時刻を進める
		mActiveColliderCount += (2.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetBattleSpeed();

		// 各コライダーの衝突判定を確認
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
	// パラメータの初期化

	// アニメーションの変更
	mActor->GetObject3D()->mSkinning->SetNextAnimation("Thrust");
	mActiveColliderCount = 0.0f;

	mIsHit = false;
	mIsOperating = true;

	// プレイヤーの方を向く

	// ここから回転処理
	// 方向の設定
	Vector3 direction = Normalize(mActor->GetWorldPosForTarget() - mActor->GetWorldPos());
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

	// 計算結果をactorクラスに渡す
	mActor->SetRotation(Vector3(0.0f, rotateY, 0.0f));

	// 刺突のときは武器を 右手 にペアレントする
	//mActor->mWeapon->mWorldTransform->SetParent(mActor->mRightHandsWorldMat);

	// 実行する
	mCondition = Condition::RUNNING;
}

void ACT::AttackThrust::End() 
{
	// 行動を終了させる
	mCondition = Condition::FINISHED;

	// アニメーションの変更
	//mActor->GetObject3D()->mSkinning->SetNextAnimation("Idle");

	// 終わったら戻す
	//mActor->mWeapon->mWorldTransform->SetParent(mActor->mRightHandWorldMat);

}

void ACT::AttackThrust::Reset()
{
	// 初期化する
	mCondition = Condition::IDOL;
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


