#pragma once
#include "ICommand.h"

class Input;

class InputHandle
{
public:
	ICommand* HandleInput();

	void AssingnMoveUpCommand2PressKeyW();
	void AssingnMoveDownCommand2PressKeyS();
	void AssingnMoveLeftCommand2PressKeyA();
	void AssingnMoveRightCommand2PressKeyD();

private:

	// 外部ポインタ
	Input* input_ = nullptr;

	ICommand* pressKeyW_;
	ICommand* pressKeyS_;
	ICommand* pressKeyA_;
	ICommand* pressKeyD_;

};

