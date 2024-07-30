#include "BTMoveAction.h"
#include "GameEngine/GameBase/Enemy/BossEnemy.h"
#include "GameEngine/GameBase/Enemy/Action/Action.h"

BT::MoveToPlayer::MoveToPlayer(BossEnemy* boss)
{
	// ボスクラスのポインタ取得
	mBoss = boss;
}

BT::NodeStatus BT::MoveToPlayer::Tick()
{
	// 実行終了状態であれば早期リターンする
	if (this->mCondition == NodeCondition::FINISHED) {
		return BT::NodeStatus::SUCCESS;
	}

	// 行動待機時有効化
	if (mBoss->GetActionCondition("MoveToPlayer") == ACT::Condition::IDOL) {
		mBoss->MoveToPlayer();
	}

	// 行動中はRUNNINGを返す
	if (mBoss->GetActionCondition("MoveToPlayer") == ACT::Condition::RUNNING) {
		// 実行中状態にする
		this->mCondition = NodeCondition::RUN;
		return BT::NodeStatus::RUNNING;
	}

	
	// 終了状態にする
	this->mCondition = NodeCondition::FINISHED;
	// 終了したらSUCCESSを返す
	return BT::NodeStatus::SUCCESS;	
}
