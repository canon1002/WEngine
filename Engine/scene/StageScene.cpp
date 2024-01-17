#include "StageScene.h"

void StageScene::Init() {
	inputHandler_ = std::make_unique<InputHandle>();

	// Assign command
	inputHandler_->AssingnMoveUpCommand2PressKeyW();
	inputHandler_->AssingnMoveDownCommand2PressKeyS();
	inputHandler_->AssingnMoveLeftCommand2PressKeyA();
	inputHandler_->AssingnMoveRightCommand2PressKeyD();
	
}

void StageScene::Update() {
	
	// get command
	command_ = inputHandler_->HandleInput();

	// set command
	if (this->command_) {
		
	}

}

void StageScene::Draw() {
	
}
