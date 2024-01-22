#include "TitleScene.h"
#include "../base/DirectXCommon.h"
#include "../Input.h"

//　継承した関数
void TitleScene::Init() {
	input_ = Input::GetInstance();
}

void TitleScene::Update() {

	ImGui::Text("TitleScene");

	if (input_->GetPushKey(DIK_SPACE)) {
		// メインゲームに切り替え
		IScene::sceneNo = STAGE;
	}
}

void TitleScene::Draw(){

}
