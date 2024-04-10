#pragma once

// 敵の状態
enum EnemyState {
	Approatch,	//接近
	Leave,		// 離脱
};

// Enemyクラスを前方宣言する
class Enemy;

/// <summary>
/// 敵の[State Pattern]基底クラス
/// </summary>
class IEnemyState{
public: // -- public メンバ関数 -- //

	/// <summary>
	/// 現在の状態を取得する
	/// </summary>
	/// <returns></returns>
	int GetStateNo();


	// 仮想デストラクタ (警告回避)
	virtual ~IEnemyState();

	/// <summary>
	/// 初期化処理
	/// <para> それぞれの状態に応じた更新処理を行うため純粋仮想関数にする </para>
	/// </summary>
	virtual void Init(Enemy* enemy) = 0;

	/// <summary>
	/// 更新処理
	/// <para> それぞれの状態に応じた更新処理を行うため純粋仮想関数にする </para>
	/// </summary>
	virtual void Update() = 0;

private: // -- private メンバ関数 -- //
public: // -- public メンバ変数 -- //

	// 状態を管理する変数
	static int StateNo;

protected: // -- protected メンバ変数 -- //

	// 敵のポインタ
	Enemy* enemy_ = nullptr;

};

