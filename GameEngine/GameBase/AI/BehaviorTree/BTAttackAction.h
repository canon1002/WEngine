#pragma once

#include "IBehavior.h"

namespace BT {

	/// --------------------------------------------------
	/// 近接攻撃
	/// --------------------------------------------------
	class AttackClose : public Action
	{
	public:

		AttackClose(BossEnemy* boss);
		virtual ~AttackClose() = default;

		// 実行
		virtual BT::NodeStatus Tick()override;

		// 再起動
		virtual void Reset() override {
			// 待機状態にする
			this->mCondition = NodeCondition::IDOL;
		}
	};

}


