#pragma once
#include "AINodeBase.h"

///
/// 条件分岐ノードをまとめたファイル
///

namespace AINode {

	/// 制御ノード 基底クラス
	/// <param name="派生先 : "> Selector,Swquence等のTreeの実行順を制御するノード </param>
	/// <param name="特徴 : "> SetChald 等の子ノードに関する共通処理を持つ </param>
	class Composite : public INode {

	public: // -- 公開 メンバ関数 -- //

		// コンストラクタ
		Composite() = default;
		// 仮想デストラクタ
		virtual ~Composite() override = default;

		// 実行処理
		virtual State Tick() override = 0;
		// 再起動処理
		virtual void Reset() override = 0;

		// 子ノードの番号をセットする
		inline virtual void SetChald(const std::shared_ptr<INode>& child) {
			mChildren.push_back(child);
		}

		// 子ノードの番号をまとめてセットする
		inline virtual void SetChaldlen(const std::vector<std::shared_ptr<INode>>& childlen) {
			for (const auto& child : childlen) {
				mChildren.push_back(child);
			}
		}

	public: // -- 公開 メンバ変数 -- //

		// 子ノード
		std::vector<std::shared_ptr<INode>> mChildren;

	};


	// セレクター
	class Selector : public Composite {

	public: // -- 公開 メンバ関数 -- //

		// コンストラクタ
		Selector(const std::string& nodeName) {};

		// デストラクタ
		virtual ~Selector()override {};

		// 実行処理
		virtual State Tick()override {

			// 実行終了状態であれば早期リターンする
			if (mState == State::SUCCESS || mState == State::FAILURE) {
				return mState;
			}

			for (auto& child : mChildren) {

				// 子ノードの処理を実行する
				State state = child->Tick();

				switch (state)
				{
					// いずれかの子ノードが成功したら成功を返す
					// また、セレクターの特徴としていずれか成功したら終了になる
				case State::SUCCESS:
					mState = State::SUCCESS;
					return mState;

					break;

					// 実行中の場合、実行中という結果を返して終了する
				case State::RUNNING:

					// 実行中状態にする
					mState  = State::RUNNING;
					return mState;

					break;
				default:
					break;
				}

			}

			// ここまで来た場合は全ての子ノードが失敗した場合なので失敗を返して終了する
			mState = State::FAILURE;
			return mState;
		}

		// 再起動
		virtual void Reset() override {
			// 待機状態にする
			mState = State::READY;
			for (auto& child : mChildren) {
				child->Reset();
			}
		}

	};

	// シークエンス
	class Sequence : public Composite {

	public: // -- 公開 メンバ関数 -- //

		// コンストラクタ
		Sequence(const std::string& nodeName){
			mName = nodeName;
		};

		// デストラクタ
		virtual ~Sequence()override{};

		// 実行処理
		virtual State Tick()override {

			// 実行終了状態であれば早期リターンする
			if (mState == State::SUCCESS || mState == State::FAILURE) {
				return mState;
			}

			for (auto& child : mChildren) {

				// 子ノードの処理を実行する
				State state = child->Tick();

				switch (state)
				{
					// いずれかの子ノードが成功したら成功を返す
					// また、シークエンスの特徴としていずれか失敗したら終了になる
				case State::FAILURE:
					mState = State::FAILURE;
					return mState;

					break;

					// 実行中の場合、実行中という結果を返して終了する
				case State::RUNNING:

					// 実行中状態にする
					mState = State::RUNNING;
					return mState;

					break;
				default:
					break;
				}

			}

			// ここまで来た場合は全ての子ノードが成功した場合なので成功を返して終了する
			mState = State::SUCCESS;
			return mState;
		}

		// 再起動
		virtual void Reset() override {
			// 待機状態にする
			mState = State::READY;
			for (auto& child : mChildren) {
				child->Reset();
			}
		}

	};

}