#include "IBehavior.h"
#include "App/Enemy/BossEnemy.h"
#include "App/Enemy/Action/Action.h"

BT::NodeStatus BT::Action::Tick()
{
	// 早期リターン
	if (this->mCondition == NodeCondition::FINISHED) {
		return BT::NodeStatus::SUCCESS;
	}

	// 行動待機時有効化
	if (mBoss->GetActionCondition(mActionName) == ACT::Condition::IDOL) {
		mBoss->SetAction(mActionName);
	}

	// 行動中はRUNNINGを返す
	if (mBoss->GetActionCondition(mActionName) == ACT::Condition::RUNNING) {
		// 実行中状態にする
		this->mCondition = NodeCondition::RUN;
		return BT::NodeStatus::RUNNING;
	}

	// 終了状態にする
	this->mCondition = NodeCondition::FINISHED;
	// 終了したらSUCCESSを返す
	return BT::NodeStatus::SUCCESS;

}
