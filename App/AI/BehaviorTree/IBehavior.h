#pragma once
#include <functional>
#include "GameEngine/Math/Math.h"
#include <iostream>

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

				// 子ノードの処理を実行する
				BT::NodeStatus status = child->Tick();

				// 失敗したら
				if (status == BT::NodeStatus::FAILURE) {
					// 終了状態にする
					this->mCondition = NodeCondition::FINISHED;
					// 一つでも失敗したら失敗
					mResultStatus = BT::NodeStatus::FAILURE;
					return mResultStatus;
				}
				// 実行中
				else if (status == BT::NodeStatus::RUNNING) {
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
				// 子ノードの処理を実行する
				BT::NodeStatus status = child->Tick();

				// 成功したら
				if (status == BT::NodeStatus::SUCCESS) {
					// 終了状態にする
					this->mCondition = NodeCondition::FINISHED;
					// 
					return BT::NodeStatus::SUCCESS;
				}
				// 実行中
				else if (status == BT::NodeStatus::RUNNING) {
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

		// コンストラクタ 
		Action(BossEnemy* boss, const std::string& actionName) {
			// ボスクラスのポインタ取得
			mBoss = boss;
			// アクション名をセット
			mActionName = actionName;
		}
		virtual ~Action() = default;

		// 実行
		virtual BT::NodeStatus Tick()override;
		// 再起動
		virtual void Reset() override {
			// 待機状態にする
			this->mCondition = NodeCondition::IDOL;
		}

	protected:

		// ボスクラスのポインタ
		BossEnemy* mBoss;

		// アクション名
		std::string mActionName;

	};

	// --------------------------------------------
	// コンディションノード(条件ノード)
	// --------------------------------------------
	class Condition : public INode {
	public:

		// コンストラクタ // 条件分岐用の関数を引数として渡す
		Condition(std::function<bool()> func) {
			this->func = func;
			//mResultStatus = BT::NodeStatus::FAILURE;
		};

		// 実行 // ⚠RUNNINGを返してはならない⚠
		virtual BT::NodeStatus Tick()override
		{
			// 実行終了状態であれば早期リターンする
			if (this->mCondition == NodeCondition::FINISHED) {
				return mResultStatus;
			}

			// 条件用の関数ポインタの結果がtureの場合は成功を返す
			if (func()) {
				// 終了状態にする
				this->mCondition = NodeCondition::FINISHED;
				// 成功
				mResultStatus = BT::NodeStatus::SUCCESS;
				return mResultStatus;
			}
			// 終了状態にする
			this->mCondition = NodeCondition::FINISHED;
			// 失敗
			mResultStatus = BT::NodeStatus::FAILURE;
			return mResultStatus;
		}

		// 再起動
		virtual void Reset() override {
			// 待機状態にする
			this->mCondition = NodeCondition::IDOL;
		}

	public:

		// 条件用 関数ポインタ
		std::function<bool()> func;

		// 実行結果を保持する
		BT::NodeStatus mResultStatus;

	};

	// --------------------------------------------
	// デコレーターノード
	// --------------------------------------------
	class Decorator : public INode {
	public:

		// コンストラクタ // 条件分岐用の関数を引数として渡す
		Decorator(std::function<bool()> func, BT::INode* child) {
			// 子ノードのセット
			this->child = child;
			// 条件用関数ポインタのセット
			this->func = func;
			// 待機状態にする
			this->mCondition = NodeCondition::IDOL;
			//mResultStatus = BT::NodeStatus::FAILURE;
			mHoldcondition = false;

		};

		// 実行
		virtual BT::NodeStatus Tick()override {

			// 実行終了状態であれば早期リターンする
			if (this->mCondition == NodeCondition::FINISHED) {
				return mResultStatus;
			}

			if (mHoldcondition!=true) {
				// 条件関数の結果を保持
				mHoldcondition = func();
			}

			// 条件用の関数ポインタの結果がtureの場合
			if (mHoldcondition==true) {

				// 条件成功したら子ノードの処理を実行する
				BT::NodeStatus status = child->Tick();

				// 成功したら
				if (status == BT::NodeStatus::SUCCESS) {
					// 終了状態にする
					this->mCondition = NodeCondition::FINISHED;
					// 
					return BT::NodeStatus::SUCCESS;
				}
				// 失敗したら
				else if (status == BT::NodeStatus::FAILURE) {
					// 終了状態にする
					this->mCondition = NodeCondition::FINISHED;
					// 一つでも失敗したら失敗
					mResultStatus = BT::NodeStatus::FAILURE;
					return mResultStatus;
				}
				// 実行中
				else if (status == BT::NodeStatus::RUNNING) {
					// 実行中状態にする
					this->mCondition = NodeCondition::RUN;
					return BT::NodeStatus::RUNNING;
				}


			}
			// 終了状態にする
			this->mCondition = NodeCondition::FINISHED;
			// 失敗
			mResultStatus = BT::NodeStatus::FAILURE;
			return mResultStatus;
		}

		// 再起動
		virtual void Reset() override {
			// 待機状態にする
			this->mCondition = NodeCondition::IDOL;
			mHoldcondition = false;
		}

	public:

		// 条件用 関数ポインタ
		std::function<bool()> func;

		// 実行結果を保持する
		BT::NodeStatus mResultStatus;

		// 子ノード
		INode* child;

		// 条件関数の結果保持
		bool mHoldcondition;

	};


	// --------------------------------------------
	// パラレル(並行思考)ノード
	// --------------------------------------------
	class Parallel : public INode {

	public:// -- 公開 メンバ関数 -- // 

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

				// 子ノードの処理を実行する
				BT::NodeStatus status = child->Tick();

				switch (status)
				{
					// いずれかの子ノードが成功したら成功を返し、終了状態にする
				case BT::NodeStatus::SUCCESS:

					// 終了状態にする
					this->mCondition = NodeCondition::FINISHED;
					mResultStatus = BT::NodeStatus::SUCCESS;
					return mResultStatus;

					break;

					// いずれかの子ノードが失敗したら失敗を返し、終了状態にする
				case BT::NodeStatus::FAILURE:

					// 終了状態にする
					this->mCondition = NodeCondition::FINISHED;
					mResultStatus = BT::NodeStatus::FAILURE;
					return mResultStatus;

					break;

					// 実行中の処理。すべての子ノードを実行するため、returnしない
				case BT::NodeStatus::RUNNING:

					// 実行中状態にする
					this->mCondition = NodeCondition::RUN;
					mResultStatus = BT::NodeStatus::RUNNING;

					break;
				default:
					break;
				}

			}
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

}
