#pragma once
#include "GameEngine/Math/Math.h"

/// <summary>
/// 攻撃段階
/// </summary>
enum AttackPhase {
	Default,//非攻撃時
	OperatingExtra, // 攻撃前の予備動作
	InAttack,// 攻撃中
	After,// 攻撃終了後
};

/// <summary>
/// 攻撃関連パラメータ
/// <para> 攻撃に関する変数をまとめる </para>
/// <para> また、キャラステータスの"STR"に依存する項目を設定する </para>
/// </summary>
struct WorkAttack {
	AttackPhase attackPhase; // 攻撃段階
	Vector2 parryReceptionTime; // 自身の攻撃に対するパリイを受け付ける時間
	bool isOperating;	// 動作中であるか(攻撃判定フラグとしても扱う)
	bool isHit;			// 攻撃が命中したか
	float inputWaitingTime; // 入力待ち段階
	bool isComboRequest; // 入力を受けたか
	int32_t comboCount; // 現在のコンボが何段目か
	float elapsedTime;
};

// 攻撃用定数
struct ConstAttack {
	float offence;			// 威力
	float operationTime;	// 予備動作時間
	float motionTimeMax;	// 終了時間
	float afterTime;		// 攻撃後の硬直切り替え時間
	float actionSpeed;		// 実行速度
	float inputWaitTime;	// 入力受付時間
};
