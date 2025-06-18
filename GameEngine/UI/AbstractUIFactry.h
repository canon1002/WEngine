#pragma once
#include "GameEngine/UI/BaseUI.h"
#include <string>
#include <memory>

/// <summary>
/// UI工場(概念)
/// </summary>
class AbstractUIFactory
{
public: // -- 公開メンバ 関数 -- //

	// 仮想デストラクタ
	virtual ~AbstractUIFactory() = default;

	// UI生成
	virtual std::unique_ptr<BaseUI> CreateUI(const std::string& type) = 0;

};