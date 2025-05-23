#include "WindowManager.h"

void WindowManager::Init(const wchar_t* windowClassName, const wchar_t* windowTitle, int32_t clientWidth, int32_t clientHeight) {
    
    // クライアント領域のサイズを取得し、設定する
    mClientWidth = clientWidth;
    mClientHeight = clientHeight;

	// ウィンドウクラスの設定
    mWc = {};
    mWc.lpfnWndProc = DefWindowProc;
    mWc.lpszClassName = windowClassName;
    mWc.hInstance = GetModuleHandle(nullptr);
    mWc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスの登録
    RegisterClass(&mWc);

	// ウィンドウのサイズを取得
    mWrc = { 0, 0, mClientWidth, mClientHeight };
    AdjustWindowRect(&mWrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウの作成
    mHwnd = CreateWindow(
        mWc.lpszClassName, windowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        mWrc.right - mWrc.left, mWrc.bottom - mWrc.top,
        nullptr, nullptr,
        mWc.hInstance, nullptr
    );

	
    ShowWindow(mHwnd, SW_SHOW);
}

void WindowManager::Final() {
    // ウィンドウを閉じる
    CloseWindow(mHwnd);
}

void WindowManager::ToggleFullscreen() {
    HMONITOR hMonitor = MonitorFromWindow(mHwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(MONITORINFO) };
    GetMonitorInfo(hMonitor, &mi);

    mIsFullscreen = !mIsFullscreen;

    if (mIsFullscreen) {
        SetWindowLongPtr(mHwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
        SetWindowPos(mHwnd, HWND_TOP,
            mi.rcMonitor.left, mi.rcMonitor.top,
            mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top,
            SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
    }
    else {
        SetWindowLongPtr(mHwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
        SetWindowPos(mHwnd, HWND_NOTOPMOST,
            CW_USEDEFAULT, CW_USEDEFAULT,
            mWrc.right - mWrc.left, mWrc.bottom - mWrc.top,
            SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
    }
}
