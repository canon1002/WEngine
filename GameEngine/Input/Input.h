#pragma once

#define DIRECTINPUT_VERSION 0x0800	// DirectInputのバージョン指定
#include <dinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include <Xinput.h>
#pragma comment (lib, "xinput.lib")

#include "./Engine/base/DirectXCommon.h"

class Input
{
private:

	Input() = default;
	~Input() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	Input(const Input& obj) = delete;
	Input& operator=(const Input& obj) = delete;

public:

	// インスタンスを取得
	static Input* GetInstance();
	// 終了処理
	void Finalize();

	// 初期化
	void Initialize();
	// 更新
	void Update();
	
	// キーの入力をチェック
	bool GetPushKey(BYTE keyNumber);	// 入力時
	bool GetPressKey(BYTE keyNumber);	// 単押し
	bool GetTriggerKey(BYTE keyNumber);	// 長押し

	IDirectInput8* directinput = nullptr;

private:

	// インスタンス
	static Input* instance;
	// 外部
	WinAPI* win_ = nullptr;
	DirectXCommon* dx_ = nullptr;

	// キーボードデバイス
	IDirectInputDevice8* keyboard = nullptr;
	// キーの入力状況
	BYTE keys[256] = {};
	BYTE preKeys[256] = {};

	// コントローラー
	static unsigned short preButton;
	static XINPUT_STATE state;
	static XINPUT_VIBRATION vibration;
};

