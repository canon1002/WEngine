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
	ModelManager::GetInstance()->LoadModel("ball.obj");

	// 3dオブジェクトの宣言
	eAxis_ = std::make_unique<Object3d>();
	eAxis_->Init();
	eAxis_->SetModel("emptyAxis.obj");
	
	ball_ = std::make_unique<Object3d>();
	ball_->Init();
	ball_->SetTranslate(Vec3{2.0f,0.0f,1.0f});
	ball_->SetModel("ball.obj");

	voxel_ = std::make_unique<VoxelParticle>();
	voxel_->Initialize();

	/*sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize();
	sprite_->SetTexture("Resources/texture/circleWhite.png");*/

}

void TitleScene::Update() {

	//eAxis_->Update();
	//ball_->Update();
	//voxel_->Update();
	//sprite_->Update();

	if (input_->GetTriggerKey(DIK_SPACE)) {
		// メインゲームに切り替え
		IScene::sceneNo = STAGE;
	}
}

void TitleScene::Draw(){

	//eAxis_->Draw();
	//ball_->Draw();

	//DirectXCommon::GetInstance()->DrawPariticleBegin();
	//voxel_->Draw();

	//sprite_->Draw();

}
