#include "BTAttackAction.h"
#include "GameEngine/GameBase/Enemy/BossEnemy.h"

BT::AttackClose::AttackClose(BossEnemy* boss)
{
	// ボスクラスのポインタ取得
	mBoss = boss;
}

BT::NodeStatus BT::AttackClose::Tick()
{
	// 早期リターン
	if (this->mCondition == NodeCondition::FINISHED) {
		return BT::NodeStatus::SUCCESS;
	}

	// 行動待機時有効化
	if (mBoss->GetActionCondition("AttackClose") == ACT::Condition::IDOL) {
		mBoss->AttackClose();
	}

	// 行動中はRUNNINGを返す
	if (mBoss->GetActionCondition("AttackClose") == ACT::Condition::RUNNING) {
		// 実行中状態にする
		this->mCondition = NodeCondition::RUN;
		return BT::NodeStatus::RUNNING;
	}

	// 終了状態にする
	this->mCondition = NodeCondition::FINISHED;
	// 終了したらSUCCESSを返す
	return BT::NodeStatus::SUCCESS;
}



