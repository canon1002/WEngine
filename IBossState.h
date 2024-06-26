#pragma once

// ボスクラスの前方宣言
class BossEnemy;

// 各状態名をenumで表記
enum BOSSSTATE {
	VITALITY,	// 元気
	INJURY,		// 負傷
	NEARDEATH,	// 瀕死
};

// ボス State Pattern 基底クラス
class IBossState {

public: // -- 公開 メンバ関数 -- //

	IBossState() = default;
	~IBossState() = default;
	
	virtual void Init() = 0;
	virtual void Update() = 0;

	inline void SetBoss(BossEnemy* boss) { mBoss = boss; }

private: // -- 非公開 メンバ変数 -- //

	// ボスクラスのポインタ
	BossEnemy* mBoss;

};

