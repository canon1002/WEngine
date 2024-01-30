#include "InputHandle.h"
#include "../Input.h"

ICommand* InputHandle::HandleInput() {
	input_ = Input::GetInstance();

	if (input_->GetPressKey(DIK_W) && input_->GetPressKey(DIK_A)) {

		return pressKeyWA_;
	}
	if (input_->GetPressKey(DIK_W) && input_->GetPressKey(DIK_D)) {
		return pressKeyWD_;
	}
	if (input_->GetPressKey(DIK_W)) {
		return pressKeyW_;
	}
	if (input_->GetPressKey(DIK_S) && input_->GetPressKey(DIK_A)) {
		return pressKeySA_;
	}
	if (input_->GetPressKey(DIK_S) && input_->GetPressKey(DIK_D)) {
		return pressKeySD_;
	}
	if (input_->GetPressKey(DIK_S)) {
		return pressKeyS_;
	}
	if (input_->GetPressKey(DIK_A)) {
		return pressKeyA_;
	}
	if (input_->GetPressKey(DIK_D)) {
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
