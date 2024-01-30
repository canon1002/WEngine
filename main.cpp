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
	
	// Scene
	std::unique_ptr<SceneManager> sceneManager = std::make_unique< SceneManager>();
	sceneManager->Run();

	return 0;
}
