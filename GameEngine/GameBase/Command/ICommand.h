#pragma once
#include "GameBase/Player/Player.h"

class ICommand {
public:
	virtual ~ICommand();
	virtual void Exec(Player& player) = 0;
};

