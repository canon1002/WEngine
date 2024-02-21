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
	eAxis_->SetTranslate({ 4.35f,1.75f,5.0f });
	
	ball_ = std::make_unique<Object3d>();
	ball_->Init();
	ball_->SetTranslate(Vec3{2.0f,0.0f,1.0f});
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

}

void TitleScene::Update() {

	eAxis_->Update();
	ball_->Update();
	voxel_->Update();
	sprite_->Update();

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
		if (input_->GetTriggerKey(DIK_RETURN)) {
			isSceneChange = true;

		}



		// スペースキーでパーティクル再表示
		if (input_->GetTriggerKey(DIK_SPACE)) {
			voxel_->Initialize();
		}

		// 手前に回転
		if (input_->GetPushKey(DIK_W)) {
			ball_->SetRotate({
				ball_->GetWorldTransform().rotate.x - 0.05f,
				ball_->GetWorldTransform().rotate.y,
				ball_->GetWorldTransform().rotate.z
				});
		}
		// 奥に回転
		if (input_->GetPushKey(DIK_S)) {
			ball_->SetRotate({
				ball_->GetWorldTransform().rotate.x + 0.05f,
				ball_->GetWorldTransform().rotate.y,
				ball_->GetWorldTransform().rotate.z
				});
		}
		// 左回転
		if (input_->GetPushKey(DIK_A)) {
			ball_->SetRotate({
				ball_->GetWorldTransform().rotate.x,
				ball_->GetWorldTransform().rotate.y + 0.05f,
				ball_->GetWorldTransform().rotate.z
				});
		}
		// 右回転
		if (input_->GetPushKey(DIK_D)) {
			ball_->SetRotate({
				ball_->GetWorldTransform().rotate.x,
				ball_->GetWorldTransform().rotate.y - 0.05f,
				ball_->GetWorldTransform().rotate.z
				});
		}

		// 右移動
		if (input_->GetPushKey(DIK_RIGHT)) {
			ball_->SetTranslate({
				ball_->GetWorldTransform().translate.x + 0.05f,
				ball_->GetWorldTransform().translate.y,
				ball_->GetWorldTransform().translate.z
				});
		}
		// 左移動
		if (input_->GetPushKey(DIK_LEFT)) {
			ball_->SetTranslate({
				ball_->GetWorldTransform().translate.x - 0.05f,
				ball_->GetWorldTransform().translate.y,
				ball_->GetWorldTransform().translate.z
				});
		}
		// 上移動
		if (input_->GetPushKey(DIK_UP)) {
			ball_->SetTranslate({
				ball_->GetWorldTransform().translate.x,
				ball_->GetWorldTransform().translate.y + 0.05f,
				ball_->GetWorldTransform().translate.z
				});
		}
		// 右移動
		if (input_->GetPushKey(DIK_DOWN)) {
			ball_->SetTranslate({
				ball_->GetWorldTransform().translate.x,
				ball_->GetWorldTransform().translate.y - 0.05f,
				ball_->GetWorldTransform().translate.z
				});
		}

	}

}

void TitleScene::Draw(){

	eAxis_->Draw();
	ball_->Draw();
	
	// 2D画像の描画開始コマンド
	SpriteCommon::GetInstance()->DrawBegin();
	sprite_->Draw();

	DirectXCommon::GetInstance()->DrawPariticleBegin();
	voxel_->Draw();

}
