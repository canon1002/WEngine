#include "GameMainScene.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Object/Sprite/SpriteCommon.h"

void GameMainScene::Finalize()
{

}

//　継承した関数
void GameMainScene::Init() {

	input_ = InputManager::GetInstance();
	camera_ = MainCamera::GetInstance();

	spriteA_ = std::make_unique<Sprite>();
	spriteA_->Initialize();
	spriteA_->SetTexture("Resources/texture/uvChecker.png");
	//spriteA_->SetTextureSize({ 64.0f,64.0f });
	//spriteA_->SetSpriteSize({ 256.0f,256.0f });
	spriteA_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	spriteB_ = std::make_unique<Sprite>();
	spriteB_->Initialize();
	spriteB_->SetTexture("Resources/texture/monsterBall.png");
	spriteB_->SetPos(Vec2(512.0f, 200.0f));
	//spriteB_->SetTextureSize({ 64.0f,64.0f });
	//spriteB_->SetSpriteSize({ 256.0f,256.0f });
	spriteB_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

}

void GameMainScene::Update() {
	// スペースキー入力でシーン切り替え
	if (InputManager::GetInstance()->GetKey()->GetTriggerKey(DIK_SPACE)) {
		sceneNo = TITLE;
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

	spriteA_->Update();
	spriteB_->Update();
}

void GameMainScene::Draw() {
	
	// 2D画像の描画開始コマンド
	SpriteCommon::GetInstance()->DrawBegin();
	spriteA_->Draw();
	spriteB_->Draw();

}
