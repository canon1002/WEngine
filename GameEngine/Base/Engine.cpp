#include "Engine.h"

void Engine::Run(){

	//
	// -- 生成 -- //	
	//

	// WinApp
	winApp_ = std::make_unique<WinAPI>();
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
	mDxCommon->Initialize(winApp_.get());
	// Scene
	sceneManager_->Init(winApp_.get(), mDxCommon);
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
