#pragma once

/// <summary>
/// ビヘイビアツリー 基底クラス
/// </summary>
class IBehavior {
public:
	IBehavior() = default;
	~IBehavior() = default;
	// 実行
	virtual bool Run() = 0;

	// メンバ関数ポインタ
	bool (IBehavior::* pFunk)();
};

