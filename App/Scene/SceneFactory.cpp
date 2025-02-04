#include "SceneFactory.h"
#include "StartScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"
#include "OverScene.h"
#include "MenuScene.h"

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& sceneName){

	// 次のシーンを生成
	std::unique_ptr<BaseScene> newScene = nullptr;

	if (sceneName == "Start") {
		newScene = std::make_unique<StartScene>();
	}
	if (sceneName == "Title") {
		newScene = std::make_unique<TitleScene>();
	}
	if (sceneName == "Game") {
		newScene = std::make_unique<GameScene>();
	}
	if (sceneName == "Result") {
		newScene = std::make_unique<ResultScene>();
	}
	if (sceneName == "Over") {
		newScene = std::make_unique<OverScene>();
	}
	if (sceneName == "Menu") {
		newScene = std::make_unique<MenuScene>();
	}

	return newScene;
}
