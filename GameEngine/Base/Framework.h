#pragma once
#include "GameEngine/Base/WinApp/WinAPI.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Scene/AbstractSceneFactory.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include <memory>

/// <summary>
///	フレームワーク
/// </summary>
class Framework{

public: // -- 公開 メンバ関数 -- //

	// 仮想デストラクタ
	virtual ~Framework() = default;

	// 実行処理
	void Run();

	// 初期化
	virtual void Init();
	// 更新処理
	virtual void Update();
	// 描画処理
	virtual void Draw() = 0;
	// 終了処理
	virtual void Final();

	// 終了チェック
	virtual bool IsEndRequest() { return mEndRequest; }

public: // -- 公開 メンバ変数 -- //

	// 終了リクエスト
	bool mEndRequest;

	// WinApp
	WinApp* mWinApp = nullptr;
	// DirectXCommon
	DirectXCommon* mDxCommon = nullptr;

	// ImGuiマネージャ
#ifdef _DEBUG
	std::unique_ptr<ImGuiManager> mImGuiManager = nullptr;
#endif // _DEBUG

protected: // -- 限定公開 メンバ変数 -- //

	// シーン工場(抽象)
	std::unique_ptr<AbstractSceneFactory> mSceneFactory;

};

