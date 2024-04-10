#pragma once
#include "IEnemyState.h"
#include <stdint.h>

/// <summary>
/// [EnemyState Pattern] 接近
/// </summary>
class EnemyStateApproach :   public IEnemyState{
public: // -- public メンバ関数 -- //

	// デストラクタ
	~EnemyStateApproach() = default;

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

	// 接近時に弾を発射する間隔
	int32_t shotCoolTIme_;

};

