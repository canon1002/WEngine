#pragma once
#include <memory>
#include "IScene.h"
#include "TitleScene.h"
#include "GameMainScene.h"
#include "ResultScene.h"

#include "../base/DirectXCommon.h"
#include "../object/camera/MainCamera.h"
#include "../primitive/Triangle.h"
#include "../primitive/Sprite.h"
#include "../resources/Section/Resource.h"
#include "../primitive/Sphere.h"
#include "../object/3d/ModelManager.h"
#include "../object/2d/SpriteCommon.h"
#include "../primitive/VoxelParticle.h"
#include "../../InputManager.h"
#include "../resources/Section/Audio.h"

class SceneManager
{
private:

	// シーンを保持するメンバ変数
	std::unique_ptr<IScene> sceneArr_[3];

	// どのシーンを呼び出すかを管理する変数
	int32_t currentSceneNo_;// 現在のシーン
	int32_t prevSceneNo_ = -1;// 前のシーン


	/// エンジン機能のポインタ

	// WindowsAPI
	WinAPI* win;
	// DirectX
	DirectXCommon* dx;
	// Input
	InputManager* inputManager;
	// Audio
	Audio* audio;
	// メインカメラ
	MainCamera* mainCamera;
	// モデルマネージャー
	ModelManager* modelManager;

	// スプライト基盤クラス
	SpriteCommon* spriteCommon;

public:

	SceneManager();
	~SceneManager();

	int Run();//　この関数でゲームループを呼び出す

};

