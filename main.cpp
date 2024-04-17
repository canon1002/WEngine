#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Base/Engine.h"
#include <d3d12.h>

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// リリースチェック
	D3DResourceLeakChecker leakCheck;
	// エンジン
	std::unique_ptr<Engine> engine_ = std::make_unique<Engine>();
	engine_->Run();
	return 0;
}
