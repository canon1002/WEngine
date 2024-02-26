#pragma once
#include "ICommand.h"

class InputManager;

class InputHandle
{
public:
	ICommand* HandleInput();

	void AssingnMoveUpCommand2PressKeyW();
	void AssingnMoveUpCommand2PressKeyWA();
	void AssingnMoveUpCommand2PressKeyWD();
	void AssingnMoveDownCommand2PressKeyS();
	void AssingnMoveDownCommand2PressKeySA();
	void AssingnMoveDownCommand2PressKeySD();
	void AssingnMoveLeftCommand2PressKeyA();
	void AssingnMoveRightCommand2PressKeyD();

private:

	// 外部ポインタ
	InputManager* input_ = nullptr;

	ICommand* pressKeyW_;
	ICommand* pressKeyWA_;
	ICommand* pressKeyWD_;
	ICommand* pressKeyS_;
	ICommand* pressKeySA_;
	ICommand* pressKeySD_;
	ICommand* pressKeyA_;
	ICommand* pressKeyD_;

};

