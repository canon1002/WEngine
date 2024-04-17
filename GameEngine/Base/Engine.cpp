#include "Engine.h"

void Engine::Run(){

	//
	// -- 生成 -- //	
	//

	// WinApp
	winApp_ = std::make_unique<WinAPI>();
	// DirectXCommon
	dxCommon_ = std::make_unique<DirectXCommon>();
	// Scene
	sceneManager_ = std::make_unique<SceneManager>();
	
	//
	// -- 初期化 -- //
	//

	// WinApp
	winApp_->Initialize();
	// DirectXCommon
	dxCommon_->Initialize(winApp_.get());
	// Scene
	sceneManager_->Init(winApp_.get(), dxCommon_.get());
	sceneManager_->Run();

	//
	// -- 解放処理 -- //
	//

	// Scene
	sceneManager_.reset();
	// DirectXCommon
	dxCommon_->Finalize();
	// WinApp
	winApp_->Finalize();
}
