#include "Engine.h"

void Engine::Run(){

	//
	// -- 生成 -- //	
	//

	// WinApp
	mWinApp = WinApp::GetInstance();
	// DirectXCommon
	mDxCommon = DirectXCommon::GetInstance();
	// Scene
	mSceneManager = std::make_unique<SceneManager>();
	
	//
	// -- 初期化 -- //
	//

	// WinApp
	mWinApp->Init();
	// DirectXCommon
	mDxCommon->Init();
	// Scene
	mSceneManager->Init();
	mSceneManager->Run();

	//
	// -- 解放処理 -- //
	//

	// Scene
	mSceneManager.reset();
	// DirectXCommon
	DirectXCommon::GetInstance()->Final();
	// WinApp
	mWinApp->Final();
}
