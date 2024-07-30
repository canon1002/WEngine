#pragma once
#include "IBehavior.h"

namespace BT {

	/// --------------------------------------------------
	/// 接近
	/// --------------------------------------------------
	class MoveToPlayer : public Action 
	{
	public:

		MoveToPlayer(BossEnemy* boss);
		virtual ~MoveToPlayer() = default;

		// 実行
		virtual BT::NodeStatus Tick()override;

		// 再起動
		virtual void Reset() override {
			// 待機状態にする
			this->mCondition = NodeCondition::IDOL;
		}

	};

}