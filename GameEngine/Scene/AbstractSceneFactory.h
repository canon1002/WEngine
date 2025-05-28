#pragma once

#include "GameEngine/Scene/BaseScene.h"
#include <string>

// シーン名
enum class SceneName {
	Start,
	Title,
	Menu,
	Game,
	Result,
	Over
};

/// <summary>
/// シーン工場(概念)
/// </summary>
class AbstractSceneFactory
{
public: // -- 公開メンバ 関数 -- //

	// 仮想デストラクタ
	virtual ~AbstractSceneFactory() = default;

	// シーン生成
	virtual std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName) = 0;

};

