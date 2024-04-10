#pragma once

// ImGui
#include "Engine/Base/WinAPI.h"
#include "Engine/Base/DirectXCommon.h"
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
public:

	static ImGuiManager* GetInstance();

	void Initialize();

	void Begin();

	void End();

	void Draw();

	void ReleseProcess();

private:

	ImGuiManager() = default;
	~ImGuiManager() = default;

public:

};
#endif // _DEBUG