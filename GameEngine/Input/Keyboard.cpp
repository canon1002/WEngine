#include "Keyboard.h"

#include "Base/DirectX/DirectXCommon.h"
#include "Editor/ImGui/ImGuiManager.h"

// staticメンバ変数で宣言したインスタンスを初期化
Keyboard* Keyboard::instance = nullptr;

// インスタンスを取得
Keyboard* Keyboard::GetInstance() {
	// 関数内staticは初めて通ったときのみ実行される
	if (instance == nullptr) {
		instance = new Keyboard;
	}
	return instance;
}

void Keyboard::Finalize()
{

}

void Keyboard::Initialize(WinApp* winApp) {
	winApp_ = winApp;
	// Directinputオブジェクトの生成
	HRESULT hr = DirectInput8Create(
		winApp_->wc.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
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
		winApp_->hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

}

void Keyboard::Input() {

	// 前フレームの入力結果を保持
	memcpy(preKeys, keys, sizeof(keys));
	// キーボード情報の取得開始
	keyboard->Acquire();
	// 全キーの入力状態を取得する
	keyboard->GetDeviceState(sizeof(keys), keys);

}

void Keyboard::DrawGUI()
{

#ifdef _DEBUG

	ImGui::Begin("KeyBoard Number");
	ImGui::Text("1 = %d", this->GetPushKey(DIK_1));
	ImGui::Text("2 = %d", this->GetPushKey(DIK_2));
	ImGui::Text("3 = %d", this->GetPushKey(DIK_3));
	ImGui::Text("4 = %d", this->GetPushKey(DIK_4));
	ImGui::Text("5 = %d", this->GetPushKey(DIK_5));
	ImGui::Text("6 = %d", this->GetPushKey(DIK_6));
	ImGui::Text("7 = %d", this->GetPushKey(DIK_7));
	ImGui::Text("8 = %d", this->GetPushKey(DIK_8));
	ImGui::Text("9 = %d", this->GetPushKey(DIK_9));
	ImGui::Text("0 = %d", this->GetPushKey(DIK_0));
	ImGui::End();

	ImGui::Begin("KeyBoard alphabet01");
	ImGui::Text("Q = %d", this->GetPushKey(DIK_Q));
	ImGui::Text("W = %d", this->GetPushKey(DIK_W));
	ImGui::Text("E = %d", this->GetPushKey(DIK_E));
	ImGui::Text("R = %d", this->GetPushKey(DIK_R));
	ImGui::Text("T = %d", this->GetPushKey(DIK_T));
	ImGui::Text("Y = %d", this->GetPushKey(DIK_Y));
	ImGui::Text("U = %d", this->GetPushKey(DIK_U));
	ImGui::Text("I = %d", this->GetPushKey(DIK_I));
	ImGui::Text("O = %d", this->GetPushKey(DIK_O));
	ImGui::Text("P = %d", this->GetPushKey(DIK_P));
	ImGui::End();

	ImGui::Begin("KeyBoard alphabet02");
	ImGui::Text("A = %d", this->GetPushKey(DIK_A));
	ImGui::Text("S = %d", this->GetPushKey(DIK_S));
	ImGui::Text("D = %d", this->GetPushKey(DIK_D));
	ImGui::Text("F = %d", this->GetPushKey(DIK_F));
	ImGui::Text("G = %d", this->GetPushKey(DIK_G));
	ImGui::Text("H = %d", this->GetPushKey(DIK_H));
	ImGui::Text("J = %d", this->GetPushKey(DIK_J));
	ImGui::Text("K = %d", this->GetPushKey(DIK_K));
	ImGui::Text("L = %d", this->GetPushKey(DIK_L));
	ImGui::End();

	ImGui::Begin("KeyBoard alphabet03");
	ImGui::Text("Z = %d", this->GetPushKey(DIK_Z));
	ImGui::Text("X = %d", this->GetPushKey(DIK_X));
	ImGui::Text("C = %d", this->GetPushKey(DIK_C));
	ImGui::Text("V = %d", this->GetPushKey(DIK_V));
	ImGui::Text("B = %d", this->GetPushKey(DIK_B));
	ImGui::Text("N = %d", this->GetPushKey(DIK_N));
	ImGui::Text("M = %d", this->GetPushKey(DIK_M));
	ImGui::End();

	ImGui::Begin("KeyBoard other");
	ImGui::Text("LShift = %d", this->GetPushKey(DIK_LSHIFT));
	ImGui::Text("LCtrl = %d", this->GetPushKey(DIK_LCONTROL));
	ImGui::Text("TAB = %d", this->GetPushKey(DIK_TAB));
	ImGui::Text("LAlt = %d", this->GetPushKey(DIK_LALT));
	ImGui::Text("SPACE = %d", this->GetPushKey(DIK_SPACE));
	ImGui::Text("RShift = %d", this->GetPushKey(DIK_RSHIFT));
	ImGui::Text("RCtrl = %d", this->GetPushKey(DIK_RCONTROL));
	ImGui::Text("RAlt = %d", this->GetPushKey(DIK_RALT));
	ImGui::Text("Left = %d", this->GetPushKey(DIK_LEFT));
	ImGui::Text("Up = %d", this->GetPushKey(DIK_UP));
	ImGui::Text("Down = %d", this->GetPushKey(DIK_DOWN));
	ImGui::Text("Right = %d", this->GetPushKey(DIK_RIGHT));
	ImGui::End();


#endif // _DEBUG

}

// キーの入力をチェック
bool Keyboard::GetPushKey(BYTE keyNumber) {

	// 指定したキーが入力されていればtrueを返す
	if (Keyboard::GetInstance()->keys[keyNumber]) {
		return true;
	}

	// そうでなければfalseを返す
	return false;
}


// キーの入力をチェック -- 押しっぱ --
bool Keyboard::GetPressKey(BYTE keyNumber)
{
	// 指定したキーが 「前のフレームで入力されている」かつ
	// 「現在のフレームで入力されている」状態であればtrueを返す
	if (Keyboard::GetInstance()->keys[keyNumber] && 
		Keyboard::GetInstance()->preKeys[keyNumber]) {
		return true;
	}

	// そうでなければfalseを返す
	return false;
}

// キーの入力をチェック -- 単押し --
bool Keyboard::GetTriggerKey(BYTE keyNumber) {

	// 指定したキーが 「前のフレームで入力されていない」かつ
	// 「現在のフレームで入力されている」状態であればtrueを返す
	if (Keyboard::GetInstance()->keys[keyNumber] && 
		!Keyboard::GetInstance()->preKeys[keyNumber]) {
		return true;
	}

	// そうでなければfalseを返す
	return false;
}