#include "AINodeActions.h"
#include "App/Actor/Actor.h"



namespace AINode {

#pragma region アクションノード

	State Action::Tick() {

		switch (mState)
		{
			// 待機時
		case AINode::State::READY:

			// アクションの実行を開始
			mActor->SetAction(mName);
			// 状態を実行中にする 
			mState = State::RUNNING;

			return mState;
			break;

			// 実行中
		case AINode::State::RUNNING:

			if (mActor->GetActionCondition(mName) == ACT::Condition::FINISHED) {
				mState = State::SUCCESS;
			}

			return mState;
			break;

			// 成功
		case AINode::State::SUCCESS:
			// 結果を返す
			return mState;
			break;

			// 失敗
		case AINode::State::FAILURE:
			// 結果を返す
			return mState;
			break;


		default:
			break;
		}

		// いずれかに引っかからない場合はエラー
		// 一旦そのままreturnする
		return mState;

	}

	void Action::Reset() {
		// 待機状態にする
		mState = State::READY;
	}

	json Action::ConvertToJson() {

		json j;
		j = json::object();
		j[mName]["index"] = mIndex;	// エディタで使用するノード番号
		j[mName]["tag"] = mTag;		// ノードの種類(クラス名)
		j[mName]["Pos"] = { mGuiPos.x,mGuiPos.y }; // ImGuiの座標

		if (mActor) {
			j[mName]["owner"] = mActor->GetName(); // ノードを所有するアクターの名前
			j[mName]["action"] = mActor->GetName(); // アクション名
		}

		return j;
	}


#pragma endregion

#pragma region コンディションノード

	// コンストラクタ
	Condition::Condition(Actor* actor, const std::string& nodeName) {
		// Actorのポインタ取得
		mActor = actor;
		// ノード名をセット
		mName = nodeName;
		// 関数ポインタを取得する
		mFunc = mActor->GetConditionFunction(mName);
	};

	// 実行処理
	State Condition::Tick() {

		// 実行前のみ条件関数を呼び出す
		if (mState != State::READY) {
			
			// 早期リターン(保持した結果を返す)
			return mState;
		}

		// アクションの実行を開始
		if (mFunc()) {

			// 成功時
			mState = State::SUCCESS;
			return mState;
		}

		// 失敗時
		mState = State::FAILURE;
		return mState;
	}

	void Condition::Reset() {
		// 待機状態にする
		mState = State::READY;
	}

	json Condition::ConvertToJson(){

		json j;
		j[mName]["name"] = mName;			// ノードの名称
		j[mName]["index"] = mIndex;			// エディタで使用するノード番号
		j[mName]["tag"] = mTag;				// ノードの種類(クラス名)
		j[mName]["Pos"] = { mGuiPos.x,mGuiPos.y }; // ImGuiの座標

		if (mActor) {
			j[mName]["owner"] = mActor->GetName(); // ノードを所有するアクターの名前
			j[mName]["action"] = mActor->GetName(); // アクション名
		}

		return j;
	}

#pragma endregion

}