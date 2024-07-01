#pragma once

/// <summary>
/// ビヘイビアツリー 基底クラス
/// </summary>
class IBehavior {
public:
	IBehavior() = default;
	virtual ~IBehavior() = default;
	// 実行
	virtual bool Run() = 0;
};

