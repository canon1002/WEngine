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

void InputManager::Initialize(WinApp* winApp) {

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

bool InputManager::GetPushKey(BYTE keyNumber){
	return keyboard->GetPushKey(keyNumber);
}

bool InputManager::GetPressKey(BYTE keyNumber){
	return keyboard->GetPressKey(keyNumber);
}

bool InputManager::GetTriggerKey(BYTE keyNumber){
	return keyboard->GetTriggerKey(keyNumber);
}

bool InputManager::GetReleseKey(BYTE keyNumber){
	keyNumber;
	return false;
}

bool InputManager::GetPused(const Gamepad::Button& type){
	return gamepad->Pushed(type);
}

bool InputManager::GetLongPush(const Gamepad::Button& type){
	return gamepad->LongPush(type);
}

bool InputManager::GetReleased(const Gamepad::Button& type){
	return gamepad->Released(type);
}

unsigned char InputManager::GetTriger(const Gamepad::Triger& type){
	return gamepad->getTriger(type);
}

short InputManager::GetStick(const Gamepad::Stick& type){
	return gamepad->getStick(type);
}
