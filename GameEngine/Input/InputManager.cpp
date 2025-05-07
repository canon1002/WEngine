#include "InputManager.h"
#include <algorithm> 
#include "App/BlackBoard.h"

// staticメンバ変数で宣言したインスタンスを初期化
InputManager* InputManager::instance = nullptr;

// インスタンスを取得
InputManager* InputManager::GetInstance() {
	if (instance == nullptr) {
		instance = new InputManager();
	}
	return instance;
}

void InputManager::Final()
{

}

void InputManager::Init() {

	// DirectInput関連の初期化
	keyboard = Keyboard::GetInstance();
	keyboard->Init();

	//gamepad = std::make_shared<Gamepad>();

}

void InputManager::Update() {

	// 前フレームの入力の保持・フレームの入力情報を取得
	gamepad->Input();
	keyboard->Input();
	
	// 攻撃（Bボタン）
	if (GetPused(Gamepad::Button::B)) {
		mInputBuffer = InputCommand{ BufferedInput::Attack, kBufferDuration };
	}
	else if (GetPused(Gamepad::Button::A)) {
		mInputBuffer = InputCommand{ BufferedInput::Avoid, kBufferDuration };
	}
	else if (GetPused(Gamepad::Button::LEFT_SHOULDER)) {
		mInputBuffer = InputCommand{ BufferedInput::Guard, kBufferDuration };
	}

	// 先行入力更新
	UpdateInputBuffer();

}

void InputManager::UpdateInputBuffer(){

	// デルタタイムを取得する
	float deltaTime = BlackBoard::GetBattleFPS();

	// バッファの寿命更新
	if (mInputBuffer.has_value()) {
		mInputBuffer->remainingTime -= deltaTime;  // フレーム依存

		if (mInputBuffer->remainingTime <= 0.0f) {
			mInputBuffer.reset();  // 消失
		}
	}


}

void InputManager::DrawGui()
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

float InputManager::GetStickRatio(const Gamepad::Stick& type, const short min)
{
	// スティックの入力の最大値
	const int kStickValueMax = 32786; // = 1.0f
	
	// 設定した最小値 == 最大値 だったら0.0fを返す
	if (std::abs(min) == kStickValueMax) { return 0.0f; }

	// スティックの入力量
	int32_t stickValue = std::abs(GetStick(type));
	float value = (float)stickValue - (float)min; // = t
	
	// (スティック入力値 - 判定下限値)が0以下であれば0.0fを返す
	if (value <= 0.0f) { return 0.0f; }

	// 値を範囲内に補正
	value = std::clamp(value, (float)min, (float)kStickValueMax);
	// 最大値を計算
	float maxValue = (float)kStickValueMax - (float)min;
	// 相対位置を計算
	value = (value / maxValue);

	// 入力した方向が左または下方向の場合、負の値にする
	if ((float)GetStick(type) < 0.0f) {
		value *= (-1.0f);
	}

	return value;

}

std::optional<BufferedInput> InputManager::ConsumeBufferedInput() {
	if (!mInputBuffer.has_value()) return std::nullopt;

	BufferedInput result = mInputBuffer->type;
	mInputBuffer.reset();
	return result;
}

bool InputManager::HasBufferedInput(BufferedInput type) const {
	return mInputBuffer.has_value() && mInputBuffer->type == type;
}