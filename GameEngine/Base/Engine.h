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

	// 実行処理
	void Run();

	WinApp* mWinApp = nullptr;
	DirectXCommon* mDxCommon = nullptr;
	std::unique_ptr<SceneManager> mSceneManager = nullptr;

};

