#include "InputHandle.h"

ICommand* InputHandle::HandleInput() {
	if (0) {
		return pressKeyW_;
	}
	if (0) {
		return pressKeyS_;
	}
	if (0) {
		return pressKeyA_;
	}
	if (0) {
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
