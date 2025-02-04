#pragma once
#include "GameEngine/Scene/AbstractSceneFactory.h"

/// <summary>
/// シーン工場
/// </summary>
class SceneFactory :
    public AbstractSceneFactory
{
public: // -- 公開 メンバ関数 -- // 

    /// <summary>
    /// シーン生成
    /// </summary>
    /// <param name="sceneName">シーン名</param>
    /// <returns>生成したシーン</returns>
    virtual std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName) override;

};

