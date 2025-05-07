#pragma once
#include "GameEngine/Math/Math.h"

/// <summary>
/// 回避関連パラメータ
/// <para> 回避に関する変数をまとめる </para>
/// <para> また、キャラステータスの"AGI"に依存する項目を設定する </para>
/// </summary>
struct WorkAvoid {

	// 回避行動リクエスト
	bool isAvoidRequest;
	// 実行中であるか
	bool isOperating = false;
	// 無敵時間発生期間
	Vector2 invinsivleCount;

	// 回避距離
	float avoidRange;
	// 回避速度
	float avoidSpeed;
	// 回避経過時間
	float elapsedTime;
	// 回避方向
	Vector3 avoidDirection = {};
};
