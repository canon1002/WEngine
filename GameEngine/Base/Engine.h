#pragma once
#include "Base/WinApp/WinApp.h"
#include "Base/DirectX/DirectXCommon.h"
#include "Scene/SceneManager.h"
#include <memory>

/// <summary>
///	シーンマネージャーがエンジンの代わりをしてる状態なのでそれ用のクラスを作成
/// </summary>
class Engine{
public:
	void Run();
	WinApp* winApp_ = nullptr;
	DirectXCommon* mDxCommon = nullptr;
	std::unique_ptr<SceneManager>sceneManager_ = nullptr;

};

