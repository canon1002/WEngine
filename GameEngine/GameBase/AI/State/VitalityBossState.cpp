#include "VitalityBossState.h"
#include "GameEngine/GameBase/AI/BehaviorTree/ActionNode.h"
#include "GameEngine/GameBase/Enemy/BossEnemy.h"

void VitalityBossState::Init(BossEnemy* boss) {

	// ポインタを渡す
	mBoss = boss;

	// Behavior Treeを構築する
	mRoot = std::make_unique<SelectorNode>();
	mRoot->AddBehavior(new ActionNode(std::bind(&BossEnemy::invokeFarDistance,mBoss)));

	// Behavior Treeの実行速度を設定
	mBehaviorRunCount = kBehaviorRunCountMax;

}

void VitalityBossState::Update() {

	// ビヘイビアツリーの実行
	mBehaviorRunCount--;
	// カウントが0になったら実行する
	if (mBehaviorRunCount <= 0) {
		// 行動選択を実行
		mRoot->Run();
		// カウントを最大に戻す
		mBehaviorRunCount = kBehaviorRunCountMax;
	}



};
