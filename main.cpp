#include "./Engine/base/DirectXCommon.h"
#include "./Engine/scene/SceneManager.h"
#include "./Engine/resources/Section/Audio.h"
#include "../object/3d/ModelCommon.h"
#include "./Input.h"
#include <d3d12.h>

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// リリースチェック
	D3DResourceLeakChecker leakCheck;
	// WindowsAPI
	WinAPI* win = WinAPI::GetInstance();
	// DirectX
	DirectXCommon* dx = DirectXCommon::GetInstance();
	// Input
	Input* input = Input::GetInstance();
	// Audio
	Audio* audio = Audio::GetInstance();

	// Scene
	SceneManager* sceneManager= new SceneManager();
	sceneManager->Run();

	return 0;
}
