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


	void Init();
	void Update();
	void Draw();

	IDirectInput8* directinput = nullptr;

private:

	// インスタンス
	static Input* instance;
	// 外部
	WinAPI* win_;
	DirectXCommon* dx_;

	// キーボードデバイス
	IDirectInputDevice8* keyboard = nullptr;

};

