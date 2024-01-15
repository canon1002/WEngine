#pragma once
#define DIRECTINPUT_VERSION 0x0800	// DirectInputのバージョン指定
#include <dinput.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include "./Engine/base/DirectXCommon.h"

class Input
{
private:

	Input();
	~Input();

public:

	// コピーコンストラクタと演算子オーバーロードの禁止
	Input(const Input& obj) = delete;
	Input& operator=(const Input& obj) = delete;

	// インスタンスを取得
	static Input* GetInstance();

	// 初期化
	void Init();
	// 更新
	void Update();
	
	// キーの入力をチェック
	bool GetPushKey(BYTE keyNumber);
	bool GetTriggerKey(BYTE keyNumber);

	IDirectInput8* directinput = nullptr;

private:

	// インスタンス
	static Input* instance;
	// 外部
	WinAPI* win_;
	DirectXCommon* dx_;

	// キーボードデバイス
	IDirectInputDevice8* keyboard = nullptr;
	// キーの入力状況
	BYTE keys[256] = {};
	BYTE preKeys[256] = {};

};

