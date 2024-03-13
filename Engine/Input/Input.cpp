#include "Input.h"

// staticメンバ変数で宣言したインスタンスを初期化
Input* Input::instance = nullptr;
XINPUT_STATE Input::state = { 0 };
XINPUT_VIBRATION Input::vibration = { 0 };
unsigned short Input::preButton = 0;

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

	////////////////////////////////////////////
	/// コントローラー
	////////////////////////////////////////////

	/// 前フレームの入力情報を保存
	preButton = state.Gamepad.wButtons;
	// コントローラー入力情報を取得
	XInputGetState(0, &state);

	//ImGui::Begin("GamePad");
	//ImGui::Text("LeftX = %d", getStick(Stick::LEFT_X));
	//ImGui::Text("LeftY = %d", getStick(Stick::LEFT_Y));
	//ImGui::Text("RightX = %d", getStick(Stick::RIGHT_X));
	//ImGui::Text("RightY = %d", getStick(Stick::RIGHT_Y));
	//ImGui::Text("UP = %d", getButton(Button::UP));
	//ImGui::Text("DOWN = %d", getButton(Button::DOWN));
	//ImGui::Text("LEFT = %d", getButton(Button::LEFT));
	//ImGui::Text("RIGHT = %d", getButton(Button::RIGHT));
	//ImGui::Text("START = %d", getButton(Button::START));
	//ImGui::Text("BACK = %d", getButton(Button::BACK));
	//ImGui::Text("LEFT_THUMB = %d", getButton(Button::LEFT_THUMB));
	//ImGui::Text("RIGHT_THUMB = %d", getButton(Button::RIGHT_THUMB));
	//ImGui::Text("LEFT_SHOULDER = %d", getButton(Button::LEFT_SHOULDER));
	//ImGui::Text("RIGHT_SHOULDER = %d", getButton(Button::RIGHT_SHOULDER));
	//ImGui::Text("A = %d", getButton(Button::A));
	//ImGui::Text("B = %d", getButton(Button::B));
	//ImGui::Text("X = %d", getButton(Button::X));
	//ImGui::Text("Y = %d", getButton(Button::Y));
	//ImGui::Text("LEFT_TRIGER = %d", getTriger(Triger::LEFT));
	//ImGui::Text("RIGHT_TRIGER = %d", getTriger(Triger::RIGHT));
	//ImGui::End();

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