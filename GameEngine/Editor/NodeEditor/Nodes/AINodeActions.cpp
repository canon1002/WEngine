#include "AINodeActions.h"
#include "App/Actor/Actor.h"

#pragma region アクションノード

//AINode::State AINode::Action::Tick() {
//
//	switch (mState)
//	{
//		// 待機時
//	case AINode::State::READY:
//
//		// アクションの実行を開始
//		mActor->SetAction(mName);
//		// 状態を実行中にする 
//		mState = State::RUNNING;
//
//		return mState;
//		break;
//
//		// 実行中
//	case AINode::State::RUNNING:
//
//		if (mActor->GetActionCondition(mName) == ACT::Condition::FINISHED) {
//			mState = State::SUCCESS;
//		}
//
//		return mState;
//		break;
//
//		// 成功
//	case AINode::State::SUCCESS:
//		// 結果を返す
//		return mState;
//		break;
//
//		// 失敗
//	case AINode::State::FAILURE:
//		// 結果を返す
//		return mState;
//		break;
//
//
//	default:
//		break;
//	}
//
//	// いずれかに引っかからない場合はエラー
//	// 一旦そのままreturnする
//	return mState;
//
//}

json AINode::Action::ConvertToJson() {

	nlohmann::json j;
	j["name"] = mName;				// ノードの名称
	j["index"] = mIndex;			// エディタで使用するノード番号
	j["tag"] = mTag;				// ノードの種類(クラス名)
	j["owner"] = mActor->GetName(); // ノードを所有するアクターの名前
	j["action"] = mActor->GetName(); // アクション名

	return j;
}

#pragma endregion

#pragma region コンディションノード


//AINode::State AINode::Condition::Tick() {
//	// 結果を返す
//	return State::SUCCESS;
//}

json AINode::Condition::ConvertToJson() {

	nlohmann::json j;
	j["name"] = mName;				// ノードの名称
	j["index"] = mIndex;			// エディタで使用するノード番号
	j["tag"] = mTag;				// ノードの種類(クラス名)
	j["owner"] = mActor->GetName(); // ノードを所有するアクターの名前
	j["func"] = mActor->GetName();	// 条件分岐の関数名

	return j;
}


#pragma endregion


