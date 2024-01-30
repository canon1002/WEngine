#include "ICommand.h"
#include "../Object/Player.h"

ICommand::~ICommand(){}

void MoveUpCommand::Exec(Player& player) {
    player.MoveUp();
}

void MoveUpLeftCommand::Exec(Player& player) {
    player.MoveUpLeft();
}

void MoveUpRightCommand::Exec(Player& player) {
    player.MoveUpRight();
}

void MoveDownCommand::Exec(Player& player) {
    player.MoveDown();
}

void MoveDownLeftCommand::Exec(Player& player) {
    player.MoveDownLeft();
}

void MoveDownRightCommand::Exec(Player& player) {
    player.MoveDownRight();
}

void MoveLeftCommand::Exec(Player& player) {
    player.MoveLeft();
}

void MoveRightCommand::Exec(Player& player) {
    player.MoveRight();
}
