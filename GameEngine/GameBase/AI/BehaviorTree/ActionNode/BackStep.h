#pragma once
#include "../IBehavior.h"

class BossEnemy;

// アクションノード
class BackStep : public IBehavior {

public: // -- 公開 メンバ関数 -- //

	// コンストラクタ // 条件分岐用の関数を引数として渡す
	BackStep(BossEnemy* boss, std::function<bool()> func);

	inline virtual bool Run() override {
		// 実際のアクションを実行する
		return true; // あるいは成功か失敗の結果を返す
	};
	// 初期化
	void Init();
	// 更新処理
	void Update();

	// メンバ関数ポインタを返す関数
	std::function<void()> Execute() override {

		// 受け取った条件に応じて行動の成否を決める
		if (func()) {
			// 成功した場合、初期化を行う
			this->Init();
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
	// バックステップ時のジャンプ力
	float mJumpPower;
	// バックステップにかかる時間
	float mBackStepTimer;

};

