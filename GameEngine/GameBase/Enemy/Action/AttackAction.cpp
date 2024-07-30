#include "Action.h"
#include "GameEngine/GameBase/Enemy/BossEnemy.h"

void ACT::AttackClose::Init(BossEnemy* boss)
{
	// ボスのポインタを取得
	mBoss = boss;

	// 初期化する
	mCondition = Condition::IDOL;

}

void ACT::AttackClose::Update()
{
	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {
		
		// 終了処理
		if (mBoss->GetObject3D()->mSkinning->IsAnimationFinished() == true) {
			mCondition = Condition::FINISHED;
		}

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
