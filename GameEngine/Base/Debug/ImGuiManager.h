#pragma once

// ImGui
#include "GameEngine/Base/WinApp/WinAPI.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "../../Externals/imgui/imgui.h"
#include "../../Externals/imgui/imgui_impl_dx12.h"
#include "../../Externals/imgui/imgui_impl_win32.h"
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

	ImGuiManager() = default;
	~ImGuiManager() = default;

	void Init();

	void Begin();

	void End();

	void Draw();

	void ReleseProcess();

};
#endif // _DEBUG