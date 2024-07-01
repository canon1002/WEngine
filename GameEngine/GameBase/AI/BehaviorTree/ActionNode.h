#include "IBehavior.h"
#include <functional>

class BossEnemy;

// アクションノード
class ActionNode : public IBehavior {
public: // -- 公開 メンバ関数 -- //

	// コンストラクタ // 条件分岐用の関数を引数として渡す
	ActionNode(std::function<bool()> func) {
		this->func = func;
	};

	inline virtual bool Run() override {
		// 受け取った条件に応じて行動の成否を決める
		if (func) {
			// 条件に合致していれば実行
			return true; 
		}
		// そうでなければ実行しない
		return false;
	};

	// メンバ関数ポインタ
	std::function<bool()> func;
};

// アクションノード
class MoveToTargetNode : public IBehavior {
public: // -- 公開 メンバ関数 -- //

	//ActionNode();

	inline virtual bool Run() override {
		// 実際のアクションを実行する
		return true; // あるいは成功か失敗の結果を返す
	};


};