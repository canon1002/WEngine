#pragma once
#include "ICommand.h"

class InputHandle
{
public:
	ICommand* HandleInput();

	void AssingnMoveUpCommand2PressKeyW();
	void AssingnMoveDownCommand2PressKeyS();
	void AssingnMoveLeftCommand2PressKeyA();
	void AssingnMoveRightCommand2PressKeyD();

private:

	ICommand* pressKeyW_;
	ICommand* pressKeyS_;
	ICommand* pressKeyA_;
	ICommand* pressKeyD_;

};

