#include "ResultScene.h"
#include "../Input.h"

//　継承した関数
void ResultScene::Finalize()
{

}

void ResultScene::Init() {

}

void ResultScene::Update() {
	if (Input::GetInstance()->GetTriggerKey(DIK_SPACE)) {
		// メインゲームに切り替え
		IScene::sceneNo = TITLE;
	}
}

void ResultScene::Draw() {
	
}
