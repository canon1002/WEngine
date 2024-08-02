#include "ResultScene.h"

void ResultScene::Finalize(){

}

void ResultScene::Init() {
	// 入力を取得する
	mInput = InputManager::GetInstance();
}

void ResultScene::Update() {

	// シーン切り替え
	// Bボタンまたは、Enterキーでシーン遷移
	if (mInput->GetPused(Gamepad::Button::B) || mInput->GetTriggerKey(DIK_RETURN)) {
		sceneNo = SCENE::TITLE;
	}

}

void ResultScene::Draw() {
	
}
