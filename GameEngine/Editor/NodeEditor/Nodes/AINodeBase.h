#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <memory>

namespace AINode {

	// ノード実行状況
	enum class State {
		READY,		// 実行準備
		RUNNING,	// 実行中
		SUCCESS,	// 成功
		FAILURE,	// 失敗
	};

	// ノードクラスインターフェース
	class INode{

	public: // -- 公開 メンバ関数 -- //
		
		// コンストラクタ
		INode() = default;
		// 仮想デストラクタ
		virtual ~INode() = 0;

		// 実行処理
		virtual State Tick() = 0;

		// 再起動処理
		virtual void Reset() = 0;

	public: // -- 公開 メンバ変数 -- //

		// 名前
		std::string mName;
		// 自身のIndex
		int32_t mIndex;
		// ノード実行状況
		State mState;
	};

}
