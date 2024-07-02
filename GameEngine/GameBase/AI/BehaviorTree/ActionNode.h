#include "IBehavior.h"

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

// アクションノード
class MoveToTargetNode : public IBehavior {

public: // -- 公開 メンバ関数 -- //

	// コンストラクタ // 条件分岐用の関数を引数として渡す
	MoveToTargetNode(BossEnemy* boss, std::function<bool()> func);

	inline virtual bool Run() override {
		// 実際のアクションを実行する
		return true; // あるいは成功か失敗の結果を返す
	};

	void Update();

	// メンバ関数ポインタを返す関数
	std::function<void()> Execute() override {

		// 受け取った条件に応じて行動の成否を決める
		if (func()) {

			// 条件に合致していればメンバ関数ポインタを返す
			return [this]() { this->Update(); };

		}
		// そうでなければ実行しない
		return nullptr;
	}

protected: // -- 限定公開 メンバ変数 -- //

	// BossEnemyクラスのポインタ
	BossEnemy* mBoss;

	// 条件内容 メンバ関数ポインタ
	std::function<bool()> func;

	// 移動の始点
	Vector3 mStartPos;
	// 移動の終点
	Vector3 mEndPos;

	// 始点から終点への移動方向
	Vector3 mDirection;
	// それぞれの軸の移動量
	Vector3 mVelocity;
	// 移動速度
	float mMoveSpeed;

};