#pragma once

// ImGui
#include "GameEngine/Base/WinApp/WinAPI.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "Externals/imgui/imgui.h"
#include "Externals/imgui/imgui_impl_dx12.h"
#include "Externals/imgui/imgui_impl_win32.h"
#include "Externals/imgui/imnodes.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


// デバッグ時のみ使用可能にする
#ifdef _DEBUG

/// <summary>
/// ImGuiマネージャークラス
/// <para> Debug時のみ使用可能 </para>
/// </summary>
class ImGuiManager
{
public: // -- 公開 メンバ関数 -- //

	// インスタンスの取得
	static ImGuiManager* GetInstance();
	// デストラクタ
	~ImGuiManager() = default;

	// 初期化
	void Init();
	// ImGuiの開始処理
	void Begin();
	// ImGuiの終了処理
	void End();
	// 描画処理
	void Draw();
	// 終了処理
	void Final();


private: // -- 非公開 メンバ関数 -- //

	// コンストラクタ
	ImGuiManager() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	ImGuiManager(const ImGuiManager& obj) = delete;
	ImGuiManager& operator=(const ImGuiManager& obj) = delete;

private: // -- 非公開 メンバ変数 -- //

	// インスタンス
	static ImGuiManager* mInstance;

};
#endif // _DEBUG