#include "Controller.h"
#include <limits.h>

#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"

XINPUT_STATE Controller::mState = { 0 };
XINPUT_VIBRATION Controller::mVibration = { 0 };
unsigned short Controller::mPreButton = 0;

void Controller::Input() {
	mPreButton = mState.Gamepad.wButtons;
	XInputGetState(0, &mState);
}

bool Controller::GetButton(const Button& type) {
	return (mState.Gamepad.wButtons >> static_cast<unsigned short>(type)) % 2 == 1;
}

bool Controller::GetPreButton(const Button& type) {
	return (mPreButton >> static_cast<unsigned short>(type)) % 2 == 1;
}

bool Controller::Pushed(const Button& type) {
	return GetButton(type) && !GetPreButton(type);
}

bool Controller::LongPush(const Button& type) {
	return GetButton(type) && GetPreButton(type);
}

bool Controller::Released(const Button& type) {
	return !GetButton(type) && GetPreButton(type);
}

unsigned char Controller::GetTriger(const Triger& type) {
	switch (type)
	{
	case Controller::Triger::LEFT:
		return mState.Gamepad.bLeftTrigger;
		break;

	case Controller::Triger::RIGHT:
		return mState.Gamepad.bRightTrigger;
		break;

	default:
		return 0;
		break;
	}
}

short Controller::GetStick(const Stick& type) {
	switch (type)
	{
	case Controller::Stick::LEFT_X:
		return mState.Gamepad.sThumbLX;
		break;

	case Controller::Stick::LEFT_Y:
		return mState.Gamepad.sThumbLY;
		break;

	case Controller::Stick::RIGHT_X:
		return mState.Gamepad.sThumbRX;
		break;

	case Controller::Stick::RIGHT_Y:
		return mState.Gamepad.sThumbRY;
		break;

	default:
		return 0;
		break;
	}
}

void Controller::IsVibration(const int& flag) {
	if (flag >= 1) {
		mVibration.wLeftMotorSpeed = USHRT_MAX;
		mVibration.wRightMotorSpeed = USHRT_MAX;
		XInputSetState(0, &mVibration);
	}
	else {
		mVibration.wLeftMotorSpeed = 0;
		mVibration.wRightMotorSpeed = 0;
		XInputSetState(0, &mVibration);
	}
}

void Controller::DrawGUI() {
#ifdef _DEBUG

	ImGui::Begin("GamePad");
	ImGui::Text("LeftX = %d", GetStick(Stick::LEFT_X));
	ImGui::Text("LeftY = %d", GetStick(Stick::LEFT_Y));
	ImGui::Text("RightX = %d", GetStick(Stick::RIGHT_X));
	ImGui::Text("RightY = %d", GetStick(Stick::RIGHT_Y));
	ImGui::Text("UP = %d", GetButton(Button::UP));
	ImGui::Text("DOWN = %d", GetButton(Button::DOWN));
	ImGui::Text("LEFT = %d", GetButton(Button::LEFT));
	ImGui::Text("RIGHT = %d", GetButton(Button::RIGHT));
	ImGui::Text("START = %d", GetButton(Button::START));
	ImGui::Text("BACK = %d", GetButton(Button::BACK));
	ImGui::Text("LEFT_THUMB = %d", GetButton(Button::LEFT_THUMB));
	ImGui::Text("RIGHT_THUMB = %d", GetButton(Button::RIGHT_THUMB));
	ImGui::Text("LEFT_SHOULDER = %d", GetButton(Button::LEFT_SHOULDER));
	ImGui::Text("RIGHT_SHOULDER = %d", GetButton(Button::RIGHT_SHOULDER));
	ImGui::Text("A = %d", GetButton(Button::A));
	ImGui::Text("B = %d", GetButton(Button::B));
	ImGui::Text("X = %d", GetButton(Button::X));
	ImGui::Text("Y = %d", GetButton(Button::Y));
	ImGui::Text("LEFT_TRIGER = %d", GetTriger(Triger::LEFT));
	ImGui::Text("RIGHT_TRIGER = %d", GetTriger(Triger::RIGHT));
	ImGui::End();

#endif // _DEBUG

}