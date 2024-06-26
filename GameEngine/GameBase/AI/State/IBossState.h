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
	// エラー対策で仮想デストラクタを宣言
	virtual ~IBossState() = default;
	
	virtual void Init() = 0;
	virtual void Update() = 0;

	// ボスのポインタをセットする
	inline void SetBoss(BossEnemy* boss) { mBoss = boss; }

	// State番号の取得
	inline int GetStateNo() { return stateNo; }

private: // -- 非公開 メンバ変数 -- //

	// ボスクラスのポインタ
	BossEnemy* mBoss;

protected: // -- 限定公開 メンバ変数 -- //

	// 現在のState番号
	static int stateNo;
};

