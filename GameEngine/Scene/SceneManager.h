#pragma once
#include <memory>
#include "IScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/object/camera/MainCamera.h"
#include "GameEngine/Object/Sprite/Triangle.h"
#include "GameEngine/Object/Sprite/Sprite.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Sprite/Sphere.h"
#include "GameEngine/object/Model/ModelManager.h"
#include "GameEngine/object/Sprite/SpriteCommon.h"
#include "GameEngine/Object/Particle/VoxelParticle.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Resource/Audio/Audio.h"

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
	WinAPI* winApp_;
	// DirectX
	DirectXCommon* dxCommon_;

#ifdef _DEBUG
	// ImGuiManager
	std::unique_ptr<ImGuiManager> imGuiManager_ = nullptr;
#endif // _DEBUG
	// Input
	InputManager* inputManager_;
	
	// Audio
	Audio* audio_;
	// メインカメラ
	std::unique_ptr<MainCamera> mainCamera_ = nullptr;
	// モデルマネージャー
	std::unique_ptr<ModelManager> modelManager_;
	// スプライト基盤クラス
	std::unique_ptr<SpriteCommon> spriteCommon_;

public:

	SceneManager();
	~SceneManager();

	void Init(WinAPI* winApp, DirectXCommon* dxCommon);
	int Run();//　この関数でゲームループを呼び出す

};
