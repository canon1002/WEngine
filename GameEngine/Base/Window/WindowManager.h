#pragma once
#include <Windows.h>
#include <stdint.h>

class WindowManager {
public:
    // 初期化・終了
    void Init(const wchar_t* windowClassName, const wchar_t* windowTitle, int32_t clientWidth, int32_t clientHeight);
    void Final();

    // フルスクリーン切り替え
    void ToggleFullscreen();

    // HWND取得
    HWND GetHwnd() const { return mHwnd; }

    // クライアント領域サイズ取得
    int32_t GetClientWidth() const { return mClientWidth; }
    int32_t GetClientHeight() const { return mClientHeight; }

private:
    HWND mHwnd = nullptr;
    WNDCLASS mWc = {};
    RECT mWrc = {};
    bool mIsFullscreen = false;
    int32_t mClientWidth = 0;
    int32_t mClientHeight = 0;
};
