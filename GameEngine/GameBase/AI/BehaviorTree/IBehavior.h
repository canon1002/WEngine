#pragma once
#include <functional>
#include "GameEngine/Math/Math.h"

class BossEnemy;

// Behavior Tree
namespace BT {

	// 駆動状態
	enum class NodeCondition {
		IDOL,	  // 待機中
		RUN,  // 実行中
		FINISHED, // 実行終了
	};

	// 実行結果
	enum class NodeStatus {
		SUCCESS, // 成功
		FAILURE, // 失敗
		RUNNING, // 実行中
	};

	// --------------------------------------------
	// ノード基底クラス
	// --------------------------------------------
	class INode
	{
	public:// -- 公開 メンバ関数 -- //

		// コンストラクタ
		INode() { mCondition = NodeCondition::IDOL; }
		// デストラクタ
		~INode() = default;
		// 実行
		virtual BT::NodeStatus Tick() = 0;
		// 再起動
		virtual void Reset() = 0;

	protected:// -- 限定公開 メンバ関数 -- //

		NodeCondition mCondition;
	};


	// --------------------------------------------
	// シーケンスノード
	// --------------------------------------------
	class Sequence : public INode
	{
	public: // -- 公開 メンバ関数 -- // 

		// 子ノードをセット
		void SetChild(BT::INode* child) {
			children.push_back(child);
		}

		// 実行
		virtual BT::NodeStatus Tick()override {
			// 実行終了状態であれば早期リターンする
			if (this->mCondition == NodeCondition::FINISHED) {
				return mResultStatus;
			}

			for (auto child : children) {
				// 失敗したら
				if (child->Tick() == BT::NodeStatus::FAILURE) {
					// 終了状態にする
					this->mCondition = NodeCondition::FINISHED;
					// 一つでも失敗したら失敗
					mResultStatus = BT::NodeStatus::FAILURE;
					return mResultStatus;
				}
				// 実行中
				else if (child->Tick() == BT::NodeStatus::RUNNING) {
					// 実行中状態にする
					this->mCondition = NodeCondition::RUN;
					// 実行中
					mResultStatus = BT::NodeStatus::RUNNING;
					return mResultStatus;
				}
			}
			// 終了状態にする
			this->mCondition = NodeCondition::FINISHED;
			// 全ての子ノードが成功した場合は成功
			mResultStatus = BT::NodeStatus::SUCCESS;
			return mResultStatus;
		}

		// 再起動
		virtual void Reset() override {
			// 待機状態にする
			this->mCondition = NodeCondition::IDOL;
			for (auto child : children) {
				child->Reset();
			}
		}

	public: // -- メンバ変数 -- //

		// 子ノード
		std::vector<INode*>children;
		// 実行結果を保持する
		BT::NodeStatus mResultStatus;
	};


	// --------------------------------------------
	// セレクターノード(フォールバック)
	// --------------------------------------------
	class Selector : public INode
	{
	public: // -- 公開 メンバ関数 -- // 

		// 子ノードをセット
		void SetChild(BT::INode* child) {
			children.push_back(child);
		}

		// 実行
		virtual BT::NodeStatus Tick()override {
			// 実行終了状態であれば早期リターンする
			if (this->mCondition == NodeCondition::FINISHED) {
				return mResultStatus;
			}

			for (auto child : children) {
				// 成功したら
				if (child->Tick() == BT::NodeStatus::SUCCESS) {
					// 終了状態にする
					this->mCondition = NodeCondition::FINISHED;
					// 
					return BT::NodeStatus::SUCCESS;
				}
				// 実行中
				else if (child->Tick() == BT::NodeStatus::RUNNING) {
					// 実行中状態にする
					this->mCondition = NodeCondition::RUN;
					return BT::NodeStatus::RUNNING;
				}
			}
			// 終了状態にする
			this->mCondition = NodeCondition::FINISHED;
			// 全ての子ノードが失敗したら終了
			return BT::NodeStatus::FAILURE;
		}

		// 再起動
		virtual void Reset() override {
			// 待機状態にする
			this->mCondition = NodeCondition::IDOL;
			for (auto child : children) {
				child->Reset();
			}
		}

	public: // -- メンバ変数 -- //

		// 子ノード
		std::vector<INode*>children;

		// 実行結果を保持する
		BT::NodeStatus mResultStatus;

	};

	// --------------------------------------------
	// アクションノード (基底クラス)
	// --------------------------------------------
	class Action : public INode {
	public:

		Action() = default;
		virtual ~Action() = default;

		// 実行
		virtual BT::NodeStatus Tick()override {
			return BT::NodeStatus::SUCCESS;
		}
		// 再起動
		virtual void Reset() override {
			// 待機状態にする
			this->mCondition = NodeCondition::IDOL;
		}

	protected:

		// ボスクラスのポインタ
		BossEnemy* mBoss;

	};

	// --------------------------------------------
	// コンディションノード(条件ノード)
	// --------------------------------------------
	class Condition : public INode {
	public:

		// コンストラクタ // 条件分岐用の関数を引数として渡す
		Condition(std::function<bool()> func)
		{
			this->func = func;
		};

		// 実行 // ⚠RUNNINGを返してはならない⚠
		virtual BT::NodeStatus Tick()override
		{
			// 条件用の関数ポインタの結果がtureの場合は成功を返す
			if (func) {
				return BT::NodeStatus::SUCCESS;
			}
			// falseの場合は失敗を返す
			return BT::NodeStatus::FAILURE;
		}

		// 再起動
		virtual void Reset() override {
			// 待機状態にする
			this->mCondition = NodeCondition::IDOL;
		}

	public:

		// 条件用 関数ポインタ
		std::function<bool()> func;

	};

	// --------------------------------------------
	// デコレーターノード
	// --------------------------------------------
	class Decorator : public INode {
	public:

		// 
		virtual BT::NodeStatus Tick()override {

			return BT::NodeStatus::SUCCESS;
		}

		// 再起動
		virtual void Reset() override {
			// 待機状態にする
			this->mCondition = NodeCondition::IDOL;
		}

	public:

		// 条件用 関数ポインタ


	};

}

/// <summary>
/// ビヘイビアツリー 基底クラス
/// </summary>
class IBehavior {
public:
	IBehavior() = default;
	virtual ~IBehavior() = default;
	// 実行
	virtual bool Run() = 0;
	virtual std::function<void()> Execute() = 0;
};

