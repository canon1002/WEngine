#pragma once
#include "IEnemyState.h"

/// <summary>
/// [EnemyState Pattern] 離脱
/// </summary>
class EnemyStateLeave :public IEnemyState {

public: // -- public メンバ関数 -- //

	// デストラクタ
	~EnemyStateLeave() = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init(Enemy* enemy)override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;

private: // -- private メンバ関数 -- //
public: // -- public メンバ変数 -- //
private: // -- private メンバ変数 -- //
};

