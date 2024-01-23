#include "TitleScene.h"
#include "../base/DirectXCommon.h"
#include "../Input.h"
#include "../object/3d/ModelCommon.h"

//　継承した関数
void TitleScene::Init() {
	input_ = Input::GetInstance();

	eAxisModel_ = std::make_unique<Model>();
	eAxisModel_->Init();
	eAxis_ = std::make_unique<Object3d>();
	eAxis_->Init();
	eAxis_->SetModel(eAxisModel_.get());
	
}

void TitleScene::Update() {

	ImGui::Text("TitleScene");

	eAxis_->Update();

	if (input_->GetPushKey(DIK_SPACE)) {
		// メインゲームに切り替え
		IScene::sceneNo = STAGE;
	}
}

void TitleScene::Draw(){

	eAxis_->Draw();

}
