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

	/// --------------------------------------------------
	/// 刺突攻撃
	/// --------------------------------------------------
	class AttackThrust : public Action
	{
	public:

		AttackThrust(BossEnemy* boss);
		virtual ~AttackThrust() = default;

		// 実行
		virtual BT::NodeStatus Tick()override;

		// 再起動
		virtual void Reset() override {
			// 待機状態にする
			this->mCondition = NodeCondition::IDOL;
		}
	};

	/// --------------------------------------------------
	/// ダッシュ攻撃
	/// --------------------------------------------------
	class AttackDash : public Action
	{
	public:

		AttackDash(BossEnemy* boss);
		virtual ~AttackDash() = default;

		// 実行
		virtual BT::NodeStatus Tick()override;

		// 再起動
		virtual void Reset() override {
			// 待機状態にする
			this->mCondition = NodeCondition::IDOL;
		}
	};

	/// --------------------------------------------------
	/// ジャンプ攻撃
	/// --------------------------------------------------
	class AttackJump : public Action
	{
	public:

		AttackJump(BossEnemy* boss);
		virtual ~AttackJump() = default;

		// 実行
		virtual BT::NodeStatus Tick()override;

		// 再起動
		virtual void Reset() override {
			// 待機状態にする
			this->mCondition = NodeCondition::IDOL;
		}
	};

}


