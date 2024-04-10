#pragma once

#include <memory>
#include "IEnemyState.h"		// Stateパターン基底クラス
#include "EnemyStateApproach.h"	// Stateパターン派生クラス // 接近状態
#include "EnemyStateLeave.h"	// Stateパターン派生クラス // 離脱状態

/// <summary>
/// 敵の状態遷移を管理するクラス
/// </summary>
class EnemyStateManager
{
public: // -- public メンバ関数 -- //
	/// <summary>
	/// コンストラクタ
	/// </summary>
	EnemyStateManager();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~EnemyStateManager();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="enemy"></param>
	void Init(Enemy* enemy);

	/// <summary>
	/// 状態に応じた更新処理を実行する 
	/// </summary>
	void StateExecution(Enemy* enemy);

private:// -- private メンバ変数  -- //

	// 状態を保持するメンバ変数の配列
	std::unique_ptr<IEnemyState> enemyStateArr_[2];

	// どの状態を呼び出すか管理する
	int currentStateNo_;// 現在の状態
	int prevStateNo_;// 前のシーン

};
