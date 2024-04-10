#pragma once
#include <memory>
#include "IScene.h"
#include "TitleScene.h"
#include "GameMainScene.h"
#include "ResultScene.h"

#include "Engine/base/DirectXCommon.h"
#include "Engine/base/ImGuiManager.h"
#include "Engine/object/camera/MainCamera.h"
#include "Engine/Object/Sprite/Triangle.h"
#include "Engine/Object/Sprite/Sprite.h"
#include "Engine/Object/Texture/Resource.h"
#include "Engine/Object/Sprite/Sphere.h"
#include "Engine/object/Model/ModelManager.h"
#include "Engine/object/Sprite/SpriteCommon.h"
#include "Engine/Object/Particle/VoxelParticle.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Object/Audio/Audio.h"

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
#ifdef _DEBUG
	// ImGuiManager
	ImGuiManager* imGuiManager_ = nullptr;
#endif // _DEBUG
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

