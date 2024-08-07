#include "Action.h"
#include "GameEngine/GameBase/Enemy/BossEnemy.h"
#include "GameEngine/Append/Collider/SphereCollider.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Append/Collider/CollisionManager.h"

void ACT::AttackClose::Init(BossEnemy* boss)
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

void ACT::AttackClose::Update()
{
	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {

		mActiveColliderCount += 2.0f / 60.0f;

	

		if (mBoss->mWeapon->mCollider->GetOnCollisionFlag() == true) {
			mActiveColliderCount += 2.0f;
		}

		// 終了処理
		if (mBoss->GetObject3D()->mSkinning->IsAnimationFinished() == true) {
			mCondition = Condition::FINISHED;
		}

	}
}

void ACT::AttackClose::Draw() {

	if (kActiveColliderCount.x < mActiveColliderCount &&
		mActiveColliderCount < kActiveColliderCount.y) {

		mBoss->mWeapon->mCollider->Draw();
	}

}

void ACT::AttackClose::Start()
{
	// パラメータの初期化

	// アニメーションの変更
	mBoss->GetObject3D()->mSkinning->Init("boss", "boss.gltf",
		mBoss->GetObject3D()->GetModel()->modelData);
	mBoss->GetObject3D()->mSkinning->ResetTime();
	mBoss->GetObject3D()->mSkinning->SetLoopMode(false);

	//// Bossの正面に攻撃判定を表示
	//Vector3 offset = { 0.0f,0.5f,1.0f };
	//// オフセットをBossの回転に合わせて回転させる
	//offset = TransformNomal(offset, mBoss->GetObject3D()->mWorldTransform->GetWorldMatrix());
	//// オフセット分ずらす
	//mWeapon->mWorldTransform->translation = mBoss->GetObject3D()->mWorldTransform->translation + offset;

	mBoss->mWeapon->mCollider->Init();
	mBoss->mWeapon->mCollider->SetWorld(mBoss->mWeapon->mWorldTransform);

	mActiveColliderCount = 0.0f;

	// 実行する
	mCondition = Condition::RUNNING;
}

void ACT::AttackClose::End()
{
	// 行動を終了させる
	mCondition = Condition::FINISHED;

	// アニメーションの変更
	mBoss->GetObject3D()->mSkinning->Init("boss", "Idle.gltf",
		mBoss->GetObject3D()->GetModel()->modelData);
}

void ACT::AttackClose::Reset()
{
	// 初期化する
	mCondition = Condition::IDOL;
}

void ACT::AttackClose::SetCollider(CollisionManager* cManager)
{
	if (kActiveColliderCount.x < mActiveColliderCount &&
		mActiveColliderCount < kActiveColliderCount.y) {

		if (mBoss->mWeapon->mCollider->GetOnCollisionFlag() == false) {
			cManager->SetCollider(mBoss->mWeapon->mCollider);
		}
	}
}
