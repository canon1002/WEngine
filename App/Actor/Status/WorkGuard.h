#pragma once
#include "GameEngine/Utility/Math/Math.h"

/// <summary>
/// 防御関連パラメータ
/// <para> 防御に関する変数をまとめる </para>
/// <para> また、キャラステータスの"VIT"に依存する項目を設定する </para>
/// </summary>
struct WorkGuard {

	// 防御行動リクエスト
	bool isGuardRequest;

	// 防御行動時間
	float guardTime;
	// 防御行動の開始カウント(この値以上で防御が発生)
	float startGuardCount;
	// ジャストガードが発生する期間
	Vector2 justGuardCount;
	// 行動移行速度
	float guardSpeed;
	// 防御時のダメージ軽減率(被弾時にこの数値を掛ける)
	float damageProtection;

};