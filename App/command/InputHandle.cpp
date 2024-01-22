#include "InputHandle.h"
#include "../Input.h"

ICommand* InputHandle::HandleInput() {
	input_ = Input::GetInstance();

	if (input_->GetPushKey(DIK_W)) {
		return pressKeyW_;
	}
	if (input_->GetPushKey(DIK_S)) {
		return pressKeyS_;
	}
	if (input_->GetPushKey(DIK_A)) {
		return pressKeyA_;
	}
	if (input_->GetPushKey(DIK_D)) {
		return pressKeyD_;
	}
	return nullptr;					
}

void InputHandle::AssingnMoveUpCommand2PressKeyW() {
	ICommand* command = new MoveUpCommand();
	this->pressKeyW_ = command;
}
void InputHandle::AssingnMoveDownCommand2PressKeyS() {
	ICommand* command = new MoveDownCommand();
	this->pressKeyS_ = command;
}

void InputHandle::AssingnMoveLeftCommand2PressKeyA() {
	ICommand* command = new MoveLeftCommand();
	this->pressKeyA_ = command;
}

void InputHandle::AssingnMoveRightCommand2PressKeyD() {
	ICommand* command = new MoveRightCommand();
	this->pressKeyD_ = command;
}
