#include "TitleScene.h"
#include "../base/DirectXCommon.h"
#include "../Input.h"
#include "../object/3d/ModelCommon.h"
#include "../3d/ModelManager.h"

void TitleScene::Finalize()
{

}

//　継承した関数
void TitleScene::Init() {
	input_ = Input::GetInstance();

	// モデルをセットする
	ModelManager::GetInstance()->LoadModel("emptyAxis.obj");

	// 3dオブジェクトの宣言
	eAxis_ = std::make_unique<Object3d>();
	eAxis_->Init();
	eAxis_->SetModel("emptyAxis.obj");
	
	//eAxis2_ = std::make_unique<Object3d>();
	//eAxis2_->Init();
	//eAxis2_->SetTranslate(Vec3{2.0f,0.0f,1.0f});
	//eAxis2_->SetModel("emptyAxis.obj");

	voxel_ = std::make_unique<VoxelParticle>();
	voxel_->Initialize();

}

void TitleScene::Update() {

	ImGui::Text("TitleScene");

	//eAxis_->Update();
	//eAxis2_->Update();
	//voxel_->Update();
	ImGui::Begin("ResetButton");
	if (ImGui::Button("VoxelParticle")) {
		voxel_->Initialize();
	}
	ImGui::End();


	if (input_->GetPushKey(DIK_SPACE)) {
		// メインゲームに切り替え
		IScene::sceneNo = STAGE;
	}
}

void TitleScene::Draw(){

	//eAxis_->Draw();
	//eAxis2_->Draw();

	DirectXCommon::GetInstance()->DrawPariticleBegin();
	voxel_->Draw();

}
