#include "../IBehavior.h"

class BossEnemy;

// アクションノード
class ActionNode : public IBehavior {
public: // -- 公開 メンバ関数 -- //

	// コンストラクタ // 条件分岐用の関数を引数として渡す
	ActionNode(const BossEnemy* boss,std::function<bool()> func) {
		mBoss = boss;
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

	virtual void Update();

	// メンバ関数ポインタを返す関数
	std::function<void()> Execute() override {

		// 受け取った条件に応じて行動の成否を決める
		if (func) {
			// 条件に合致していればメンバ関数ポインタを返す
			return []() {
				return &ActionNode::Update;
				};
		}
		// そうでなければ実行しない
		return nullptr;
	}

protected: // -- 限定公開 メンバ変数 -- //

	// BossEnemyクラスのポインタ
	const BossEnemy* mBoss;

	// 条件内容 メンバ関数ポインタ
	std::function<bool()> func;

};
