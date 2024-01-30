#include "Input.h"

// staticメンバ変数で宣言したインスタンスを初期化
Input* Input::instance = nullptr;

// インスタンスを取得
Input* Input::GetInstance() {
	// 関数内staticは初めて通ったときのみ実行される
	if (instance == nullptr) {
		instance = new Input;
	}
	return instance;
}

void Input::Finalize()
{

}

void Input::Initialize() {

	win_ = WinAPI::GetInstance();
	dx_ = DirectXCommon::GetInstance();

	// DirectInputオブジェクトの生成
	HRESULT hr = DirectInput8Create(
		win_->wc.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directinput, nullptr);
	assert(SUCCEEDED(hr));

	////////////////////////////////////////
	// キーボードデバイスの生成
	////////////////////////////////////////

	hr = directinput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(hr));
	// 入力データ形式のセット
	hr = keyboard->SetDataFormat(&c_dfDIKeyboard);// 標準形式	
	assert(SUCCEEDED(hr));
	// 排他制御レベルのセット
	hr = keyboard->SetCooperativeLevel(
		win_->hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

}

void Input::Update() {

	// 前フレームの入力結果を保持
	memcpy(preKeys, keys, sizeof(keys));
	// キーボード情報の取得開始
	keyboard->Acquire();
	// 全キーの入力状態を取得する
	keyboard->GetDeviceState(sizeof(keys), keys);

	if (GetPushKey(DIK_0)) {
		OutputDebugStringA("Trigger 0\n");
	}

}

// キーの入力をチェック
bool Input::GetPushKey(BYTE keyNumber) {

	// 指定したキーが入力されていればtrueを返す
	if (keys[keyNumber]) {
		return true;
	}

	// そうでなければfalseを返す
	return false;
}


// キーの入力をチェック -- 押しっぱ --
bool Input::GetPressKey(BYTE keyNumber)
{
	// 指定したキーが 「前のフレームで入力されている」かつ
	// 「現在のフレームで入力されている」状態であればtrueを返す
	if (keys[keyNumber] && preKeys[keyNumber]) {
		return true;
	}

	// そうでなければfalseを返す
	return false;
}

// キーの入力をチェック -- 単押し --
bool Input::GetTriggerKey(BYTE keyNumber) {

	// 指定したキーが 「前のフレームで入力されていない」かつ
	// 「現在のフレームで入力されている」状態であればtrueを返す
	if (keys[keyNumber] && !preKeys[keyNumber]) {
		return true;
	}

	// そうでなければfalseを返す
	return false;
}