#include "TitleScene.h"
#include "../base/DirectXCommon.h"
#include "../InputManager.h"
#include "../object/3d/ModelCommon.h"
#include "../3d/ModelManager.h"

void TitleScene::Finalize()
{

}

//　継承した関数
void TitleScene::Init() {
	
	input_->GetInstance();

	// モデルをセットする
	ModelManager::GetInstance()->LoadModel("emptyAxis.obj");
	ModelManager::GetInstance()->LoadModel("ball.obj");

	// 3dオブジェクトの宣言
	eAxis_ = std::make_unique<Object3d>();
	eAxis_->Init();
	eAxis_->SetModel("emptyAxis.obj");
	eAxis_->SetTranslate({ 4.35f,1.75f,5.0f });
	
	ball_ = std::make_unique<Object3d>();
	ball_->Init();
	ball_->SetTranslate(Vec3{0.0f,0.0f,1.0f});
	ball_->SetModel("ball.obj");

	voxel_ = std::make_unique<VoxelParticle>();
	voxel_->Initialize();

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize();
	sprite_->SetTexture("Resources/texture/uvChecker.png");
	sprite_->SetTextureSize({ 64.0f,64.0f });
	sprite_->SetSpriteSize({ 1280.0f,720.0f });
	sprite_->SetColor({ 0.0f,0.0f,0.0f,0.0f });

	// シーン切り替え用
	alpth_ = 0.0f;
	isSceneChange = false;
	fadeTimer_ = 60;

	titleUI01 = std::make_unique<Sprite>();
	titleUI01->Initialize();
	titleUI01->SetTexture("Resources/texture/TitleUI01.png");
	titleUI01->SetTextureSize({ 512.0f,96.0f });
	titleUI01->SetSpriteSize({ 512.0f,96.0f });
	titleUI01->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	titleUI02 = std::make_unique<Sprite>();
	titleUI02->Initialize();
	titleUI02->SetTexture("Resources/texture/TitleUI02.png");
	titleUI02->SetTextureSize({ 512.0f,96.0f });
	titleUI02->SetSpriteSize({ 512.0f,96.0f });
	titleUI02->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	
	UI00 = std::make_unique<Sprite>();
	UI00->Initialize();
	UI00->SetTexture("Resources/texture/UI00.png");
	UI00->SetTextureSize({ 1280.0f,720.0f });
	UI00->SetSpriteSize({ 1280.0f,720.0f });
	UI00->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	UItimeCount = 15;
	isSelect = false;

}

void TitleScene::Update() {

	eAxis_->UpdateWorldMat();
	eAxis_->Update();
	Vec3 newRotate = { 0.0f,ball_->GetWorldTransform().rotate.y + 0.01f,0.0f };
	ball_->SetRotate(newRotate);
	ball_->UpdateWorldMat();
	ball_->Update();
	voxel_->Update();
	sprite_->Update();

	titleUI01->Update();
	titleUI02->Update();
	UI00->Update();

	if (isSceneChange == true) {
		if (alpth_ < 1.0f) {
			alpth_ += 0.05f;
		}
		sprite_->SetColor({ 0.0f,0.0f,0.0f,alpth_ });

		if (alpth_ >= 1.0f) {

			sceneNo = STAGE;
		}
	}
	else {

		//
		if (input_->GetKey()->GetTriggerKey(DIK_RETURN)||
			(Gamepad::getTriger(Gamepad::Triger::RIGHT))
			) {
			isSelect = true;

		}

	}
	if (isSelect && UItimeCount > 0) {
		UItimeCount--;
	}

	if (UItimeCount == 0) {
		isSceneChange = true;
	}

}

void TitleScene::Draw(){

	eAxis_->Draw();
	ball_->Draw();
	
	// 2D画像の描画開始コマンド
	SpriteCommon::GetInstance()->DrawBegin();
	sprite_->Draw();

	if (UItimeCount >= 15 ||
		(UItimeCount <= 10 && UItimeCount >= 5) ||
		UItimeCount == 0) {
		titleUI01->Draw();
	}
	else {
		titleUI02->Draw();
		
	}

	UI00->Draw();

	DirectXCommon::GetInstance()->DrawPariticleBegin();
	voxel_->Draw();

}
