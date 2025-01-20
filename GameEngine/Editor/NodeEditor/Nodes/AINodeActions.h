#pragma once
#include "AINodeBase.h"
#include <functional>

///
/// アクションノードをまとめたファイル
///

// 前方宣言
class Actor;

namespace AINode {

	/// タスクノード 基底クラス
	/// <param name="派生先 : "> Action,Condition等の実行処理を持つノード </param>
	/// <param name="特徴 : "> 子ノードを持たず、ゲームキャラとのつながりを持つノード </param>
	class Task : public INode {

	public: // -- 公開 メンバ関数 -- //

		// コンストラクタ
		Task() = default;
		// 仮想デストラクタ
		virtual ~Task() override {};

		// 実行処理
		virtual State Tick() override = 0;
		// 再起動処理
		virtual void Reset() override = 0;

	public: // -- 公開 メンバ変数 -- //
	
		// ゲームキャラクターのポインタ
		Actor* mActor;

	};

	
	// アクションノード
	class Action : public Task {
	public:

		// コンストラクタ 
		Action(Actor* actor, const std::string& nodeName) {
			// Actorのポインタ取得
			mActor = actor;
			// ノード名をセット
			mName = nodeName;
		}
		virtual ~Action() = default;

		// 実行処理
		virtual State Tick() override = 0;
		// 再起動処理
		virtual void Reset() override = 0;

	};

	// --------------------------------------------
	// コンディションノード(条件ノード)
	// --------------------------------------------
	class Condition : public INode {
	public:

		// コンストラクタ // 条件分岐用の関数を引数として渡す
		Condition(std::function<bool()> func,const std::string& nodeName) {
			mFunc = func;
			// ノード名をセット
			mName = nodeName;
		};

		// 実行処理3
		virtual State Tick() override = 0;
		// 再起動処理
		virtual void Reset() override = 0;

	public:

		// 条件用 関数ポインタ
		std::function<bool()> mFunc;

	};

}