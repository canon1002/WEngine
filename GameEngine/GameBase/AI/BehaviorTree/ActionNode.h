#include "IBehavior.h"

// アクションノード
class ActionNode : public IBehavior {
public: // -- 公開 メンバ関数 -- //

	//ActionNode();

	inline virtual bool Run() override {
		// 実際のアクションを実行する
		return true; // あるいは成功か失敗の結果を返す
	};


};