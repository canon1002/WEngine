#include "ICommand.h"
#include "../Object/Player.h"

ICommand::~ICommand(){}

void MoveUpCommand::Exec(Player& player) {
    player.MoveUp();
}

void MoveDownCommand::Exec(Player& player) {
    player.MoveDown();
}

void MoveLeftCommand::Exec(Player& player) {
    player.MoveLeft();
}

void MoveRightCommand::Exec(Player& player) {
    player.MoveRight();
}
