#pragma once
#include "GameEngine/Base/WinApp/WinAPI.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Scene/SceneManager.h"
#include <memory>
/// <summary>
///	シーンマネージャーがエンジンの代わりをしてる状態なのでそれ用のクラスを作成
/// </summary>
class Engine{
public:
	void Run();
	std::unique_ptr<WinAPI>winApp_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;
	std::unique_ptr<SceneManager>sceneManager_ = nullptr;

};

