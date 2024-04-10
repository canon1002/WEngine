#include "TitleScene.h"
#include "Engine/Object/Model/ModelManager.h"

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
}

void TitleScene::Update() {

	// スペースキー入力でシーン切り替え
	if (input_->GetKey()->GetTriggerKey(DIK_SPACE)) {
		sceneNo = STAGE;
	}

	if (input_->GetKey()->GetPushKey(DIK_W)) {
		camera_->SetTranslate(Vec3(
			camera_->GetTranslate().x,
			camera_->GetTranslate().y + 0.5f,
			camera_->GetTranslate().z
		));
	}

	if (input_->GetKey()->GetPushKey(DIK_A)) {
		camera_->SetTranslate(Vec3(
			camera_->GetTranslate().x - 0.5f,
			camera_->GetTranslate().y,
			camera_->GetTranslate().z
		));
	}

	if (input_->GetKey()->GetPushKey(DIK_S)) {
		camera_->SetTranslate(Vec3(
			camera_->GetTranslate().x,
			camera_->GetTranslate().y - 0.5f,
			camera_->GetTranslate().z
		));
	}

	if (input_->GetKey()->GetPushKey(DIK_D)) {
		camera_->SetTranslate(Vec3(
			camera_->GetTranslate().x + 0.5f,
			camera_->GetTranslate().y,
			camera_->GetTranslate().z
		));
	}

	if (input_->GetKey()->GetPushKey(DIK_Q)) {
		camera_->SetTranslate(Vec3(
			camera_->GetTranslate().x,
			camera_->GetTranslate().y,
			camera_->GetTranslate().z + 0.5f
		));
	}

	if (input_->GetKey()->GetPushKey(DIK_E)) {
		camera_->SetTranslate(Vec3(
			camera_->GetTranslate().x,
			camera_->GetTranslate().y,
			camera_->GetTranslate().z - 0.5f
		));
	}

	eAxis_->UpdateWorldMat();
	eAxis_->Update();
	Vec3 newRotate = { 0.0f,ball_->GetWorldTransform().rotate.y + 0.01f,0.0f };
	ball_->SetRotate(newRotate);
	ball_->UpdateWorldMat();
	ball_->Update();

	//particleVox->Update();
	//particleCiacle->Update();
}

void TitleScene::Draw(){

	eAxis_->Draw();
	ball_->Draw();

	// パーティクル描画
	DirectXCommon::GetInstance()->DrawPariticleBegin();
	//particleVox->Draw();
	//particleCiacle->Draw();
}
