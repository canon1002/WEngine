#include "Engine.h"

void Engine::Run(){

	//
	// -- 生成 -- //	
	//

	// WinApp
	winApp_ = WinApp::GetInstance();
	// DirectXCommon
	mDxCommon = DirectXCommon::GetInstance();
	// Scene
	sceneManager_ = std::make_unique<SceneManager>();
	
	//
	// -- 初期化 -- //
	//

	// WinApp
	winApp_->Initialize();
	// DirectXCommon
	mDxCommon->Initialize(winApp_);
	// Scene
	sceneManager_->Init(winApp_, mDxCommon);
	
	//
	// -- 実行処理 -- //
	//

	// Scene 実行
	sceneManager_->Run();

	//
	// -- 解放処理 -- //
	//

	// Scene
	sceneManager_.reset();
	// DirectXCommon
	mDxCommon->Finalize();
	// WinApp
	winApp_->Finalize();
}
