#include "InputHandle.h"
#include "../InputManager.h"

ICommand* InputHandle::HandleInput() {
	input_ = InputManager::GetInstance();

	if ((input_->GetKey()->GetPressKey(DIK_W) && input_->GetKey()->GetPressKey(DIK_A))||
		(Gamepad::getStick((Gamepad::Stick::LEFT_X)) < -3000&& Gamepad::getStick((Gamepad::Stick::LEFT_Y)) > 3000)
	){

		return pressKeyWA_;
	}
	if ((input_->GetKey()->GetPressKey(DIK_W) && input_->GetKey()->GetPressKey(DIK_D)) ||
		(Gamepad::getStick((Gamepad::Stick::LEFT_X)) > 3000 && Gamepad::getStick((Gamepad::Stick::LEFT_Y)) > 3000)
	){
		return pressKeyWD_;
	}
	if (input_->GetKey()->GetPressKey(DIK_W) ||
		Gamepad::getStick((Gamepad::Stick::LEFT_Y)) > 3000) {
		return pressKeyW_;
	}
	if ((input_->GetKey()->GetPressKey(DIK_S) && input_->GetKey()->GetPressKey(DIK_A)) ||
		(Gamepad::getStick((Gamepad::Stick::LEFT_X)) < -3000 && Gamepad::getStick((Gamepad::Stick::LEFT_Y)) < -3000)
		){
		return pressKeySA_;
	}
	if ((input_->GetKey()->GetPressKey(DIK_S) && input_->GetKey()->GetPressKey(DIK_D)) ||
		(Gamepad::getStick((Gamepad::Stick::LEFT_X)) > 3000 && Gamepad::getStick((Gamepad::Stick::LEFT_Y)) < -3000)
		){
		return pressKeySD_;
	}
	if (input_->GetKey()->GetPressKey(DIK_S) ||
		Gamepad::getStick((Gamepad::Stick::LEFT_Y)) < -3000) {
		return pressKeyS_;
	}
	if (input_->GetKey()->GetPressKey(DIK_A) ||
		Gamepad::getStick((Gamepad::Stick::LEFT_X)) < -3000) {
		return pressKeyA_;
	}
	if (input_->GetKey()->GetPressKey(DIK_D) ||
		Gamepad::getStick((Gamepad::Stick::LEFT_X)) > 3000) {
		return pressKeyD_;
	}
	return nullptr;					
}

void InputHandle::AssingnMoveUpCommand2PressKeyW() {
	ICommand* command = new MoveUpCommand();
	this->pressKeyW_ = command;
}
void InputHandle::AssingnMoveUpCommand2PressKeyWA()
{
	ICommand* command = new MoveUpLeftCommand();
	this->pressKeyWA_ = command;
}
void InputHandle::AssingnMoveUpCommand2PressKeyWD()
{
	ICommand* command = new MoveUpRightCommand();
	this->pressKeyWD_ = command;
}

void InputHandle::AssingnMoveDownCommand2PressKeyS() {
	ICommand* command = new MoveDownCommand();
	this->pressKeyS_ = command;
}
void InputHandle::AssingnMoveDownCommand2PressKeySA()
{
	ICommand* command = new MoveDownLeftCommand();
	this->pressKeySA_ = command;
}
void InputHandle::AssingnMoveDownCommand2PressKeySD()
{
	ICommand* command = new MoveDownRightCommand();
	this->pressKeySD_ = command;
}

void InputHandle::AssingnMoveLeftCommand2PressKeyA() {
	ICommand* command = new MoveLeftCommand();
	this->pressKeyA_ = command;
}

void InputHandle::AssingnMoveRightCommand2PressKeyD() {
	ICommand* command = new MoveRightCommand();
	this->pressKeyD_ = command;
}
