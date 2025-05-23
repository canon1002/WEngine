#pragma once
#include "GameEngine/UI/AbstractUIFactry.h"

// UIの振る舞い方・タイプ
enum class UIType
{
	Base,//通常
};

/// <summary>
/// UI工場(具象)
/// </summary>
class UIFactory :
    public AbstractUIFactory
{
public: // -- 公開 メンバ関数 -- // 

    /// <summary>
    /// UI生成
    /// </summary>
    /// <param name="type">UIタイプ</param>
    /// <returns>生成したシーン</returns>
    virtual std::unique_ptr<BaseUI> CreateUI(const std::string& type) override;

};

