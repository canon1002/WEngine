#pragma once
#include <memory>
#include "IScene.h"
#include "StartScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"
#include "OverScene.h"
#include "MenuScene.h"

#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/object/camera/MainCamera.h"
#include "GameEngine/Object/Sprite/Triangle.h"
#include "GameEngine/Object/Sprite/Sprite.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Sprite/Sphere.h"

#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Resource/Audio/Audio.h"

#include "GameEngine/Object/ObjectAdministrator.h"

#include "GameEngine/Object/Screen/RenderCopyImage.h"

class SceneManager
{
private:

	// シーンを保持するメンバ変数
	std::array<std::unique_ptr<IScene>, 6> sceneArr_;

	// どのシーンを呼び出すかを管理する変数
	int32_t currentSceneNo_;// 現在のシーン
	int32_t prevSceneNo_ = -1;// 前のシーン


	/// エンジン機能のポインタ

	// WindowsAPI
	WinAPI* winApp_;
	// DirectX
	DirectXCommon* mDxCommon;

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
	
	// オブジェクト管理者クラス
	ObjectManager* objectAdmin_;

	// ポストエフェクト
	RenderCopyImage* copyImage_;

public:

	SceneManager();
	~SceneManager();

	void Init(WinAPI* winApp, DirectXCommon* dxCommon);
	int Run();//　この関数でゲームループを呼び出す

	


};

