#pragma once

#define DIRECTINPUT_VERSION 0x0800	// DirectInputのバージョン指定
#include <dinput.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include "Base/WinApp/WinApp.h"

class Keyboard
{
private:

	Keyboard() = default;
	~Keyboard() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	Keyboard(const Keyboard& obj) = delete;
	Keyboard& operator=(const Keyboard& obj) = delete;

public:

	// インスタンスを取得
	static Keyboard* GetInstance();
	// 終了処理
	void Finalize();

	// 初期化
	void Init(WinApp* winApp);
	// 更新
	void Input();
	// デバッグUIの表示
	void DrawGUI();


	// キーの入力をチェック
	bool GetPushKey(BYTE keyNumber);	// 入力時
	bool GetPressKey(BYTE keyNumber);	// 単押し
	bool GetTriggerKey(BYTE keyNumber);	// 長押し

	IDirectInput8* directinput = nullptr;


private:

	WinApp* winApp_ = nullptr;

	// インスタンス
	static Keyboard* instance;

	// キーボードデバイス
	IDirectInputDevice8* keyboard = nullptr;
	// キーの入力状況
	BYTE keys[256] = {};
	BYTE preKeys[256] = {};

	//XINPUT_KEYSTROKE keyState;

};

