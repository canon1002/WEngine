#pragma once
#include <functional>
#include "GameEngine/Math/Math.h"

class BossEnemy;

/// <summary>
/// ビヘイビアツリー 基底クラス
/// </summary>
class IBehavior {
public:
	IBehavior() = default;
	virtual ~IBehavior() = default;
	// 実行
	virtual bool Run() = 0;
	virtual std::function<void()> Execute() = 0;
};

