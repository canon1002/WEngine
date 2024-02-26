#include "ResultScene.h"
#include "../InputManager.h"

//　継承した関数
void ResultScene::Finalize()
{

}

void ResultScene::Init() {

	UI01 = std::make_unique<Sprite>();
	UI01->Initialize();
	UI01->SetTexture("Resources/texture/UI01.png");
	UI01->SetTextureSize({ 1280.0f,720.0f });
	UI01->SetSpriteSize({ 1280.0f,720.0f });
	UI01->SetColor({ 1.0f,1.0f,1.0f,1.0f });

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

void ResultScene::Update() {
	UI01->Update();
	sprite_->Update();

	if (isSceneChange == true) {
		if (alpth_ < 1.0f) {
			alpth_ += 0.05f;
		}
		sprite_->SetColor({ 0.0f,0.0f,0.0f,alpth_ });

		if (alpth_ >= 1.0f) {

			sceneNo = TITLE;
		}
	}
	else {

		if (InputManager::GetInstance()->GetKey()->GetTriggerKey(DIK_SPACE) ||
			(Gamepad::getTriger(Gamepad::Triger::RIGHT))) {
			isSceneChange = true;
		}
	}

}

void ResultScene::Draw() {
	// 2D画像の描画開始コマンド
	SpriteCommon::GetInstance()->DrawBegin();
	UI01->Draw();
	sprite_->Draw();
}
