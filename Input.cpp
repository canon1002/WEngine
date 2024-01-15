#include "Input.h"

// staticメンバ変数で宣言したインスタンスを初期化
Input* Input::instance = nullptr;

Input::Input() {}
Input::~Input() {}

// インスタンスを取得
Input* Input::GetInstance() {
	// 関数内staticは初めて通ったときのみ実行される
	if (instance == nullptr) {
		instance = new Input;
	}
	return instance;
}

void Input::Init() {

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

	// キーボード情報の取得開始
	keyboard->Acquire();

	// 全キーの入力状態を取得する
	BYTE key[256] = {};
	keyboard->GetDeviceState(sizeof(key), key);


}