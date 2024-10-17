#pragma once
#include <memory>
#include "IScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

#include "Base/DirectX/DirectXCommon.h"
#include "Editor/ImGui/ImGuiManager.h"
#include "object/camera/MainCamera.h"
#include "Object/Sprite/Triangle.h"
#include "Object/Sprite/Sprite.h"
#include "Resource/Base/Resource.h"
#include "Object/Sprite/Sphere.h"

#include "Input/InputManager.h"
#include "Resource/Audio/Audio.h"

#include "Object/Screen/RenderCopyImage.h"

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
	WinApp* winApp_;
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
	
	// ポストエフェクト
	PostEffects* copyImage_;

public:

	SceneManager();
	~SceneManager();

	void Init(WinApp* winApp, DirectXCommon* dxCommon);
	int Run();//　この関数でゲームループを呼び出す

	


};

