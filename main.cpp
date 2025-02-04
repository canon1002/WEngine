#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "App/Gladiator.h"
#include <d3d12.h>

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	// リリースチェック
	D3DResourceLeakChecker leakCheck;
	
	// ゲームクラスをを呼び出す
	std::unique_ptr<Framework> game= std::make_unique<Gladiator>();
	game->Run();

	return 0;
}
