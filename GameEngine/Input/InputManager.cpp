#include "InputManager.h"


// staticメンバ変数で宣言したインスタンスを初期化
InputManager* InputManager::instance = nullptr;

// インスタンスを取得
InputManager* InputManager::GetInstance() {
	// 関数内staticは初めて通ったときのみ実行される
	if (instance == nullptr) {
		instance = new InputManager;
	}
	return instance;
}

void InputManager::Finalize()
{

}

void InputManager::Initialize(WinAPI* winApp) {

	winApp_ = winApp;
	// DirectInput関連の初期化
	keyboard = Keyboard::GetInstance();
	keyboard->Initialize(winApp_);

}

void InputManager::Update() {

	// 前フレームの入力の保持・フレームの入力情報を取得
	gamepad->Input();
	keyboard->Input();
	
}

void InputManager::DrawGUI()
{
	gamepad->DrawGUI();
	keyboard->DrawGUI();
}
