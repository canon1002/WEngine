#include "TitleScene.h"
#include "Engine/Object/Model/ModelManager.h"
#include "Engine/Object/Sprite/SpriteCommon.h"

void TitleScene::Finalize(){}

//　継承した関数
void TitleScene::Init() {

	input_ = InputManager::GetInstance();
	camera_ = MainCamera::GetInstance();

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
	ball_->SetTranslate(Vec3{ 0.0f,0.0f,1.0f });
	ball_->SetModel("ball.obj");

	/*particleVox = new VoxelParticle();
	particleVox->Initialize();
	
	particleCiacle = new VoxelParticle();
	particleCiacle->Initialize();
	particleCiacle->SetTexture(DirectXCommon::GetInstance()->srv_->defaultTexId_);*/

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize();
	sprite_->SetTexture("Resources/texture/uvChecker.png");
	sprite_->SetTextureSize({ 64.0f,64.0f });
	sprite_->SetSpriteSize({ 1280.0f,720.0f });
	sprite_->SetColor({ 0.0f,0.0f,0.0f,0.0f });
	sprite_->SetAnchorPoint(Vec2(0.0f, 0.0f));
	sprite_->SetPos(Vec2(0.0f, 0.0f));

	// シーン切り替え用
	alpth_ = 0.0f;
	isSceneChange = false;
	fadeTimer_ = 60;


}

void TitleScene::Update() {

	// スペースキー入力でシーン切り替え
	if (input_->GetKey()->GetTriggerKey(DIK_SPACE)) {
		//sceneNo = STAGE;
	}

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
		if (input_->GetKey()->GetTriggerKey(DIK_RETURN) ||
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


	eAxis_->UpdateWorldMat();
	eAxis_->Update();
	Vec3 newRotate = { 0.0f,ball_->GetWorldTransform().rotate.y + 0.01f,0.0f };
	ball_->SetRotate(newRotate);
	ball_->UpdateWorldMat();
	ball_->Update();
	sprite_->Update();

	//particleVox->Update();
	//particleCiacle->Update();
}

void TitleScene::Draw(){

	eAxis_->Draw();
	ball_->Draw();

	// 2D画像の描画開始コマンド
	SpriteCommon::GetInstance()->DrawBegin();
	sprite_->Draw();

	// パーティクル描画
	DirectXCommon::GetInstance()->DrawPariticleBegin();
	//particleVox->Draw();
	//particleCiacle->Draw();
}
