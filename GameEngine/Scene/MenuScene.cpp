#include "MenuScene.h"
#include "GameEngine/Object/Screen/RenderCopyImage.h"
#include "GameEngine/GameMaster/Framerate.h"

void MenuScene::Finalize(){

}

void MenuScene::Init() {
	// 入力を取得する
	mInput = InputManager::GetInstance();

	mIsFading = true;
	mViggnetTime = 0.0f;
}

void MenuScene::Update() 
{
	if (mIsFading)
	{
		if (mViggnetTime < 1.0f)
		{
			mViggnetTime += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
		}
		else if (mViggnetTime >= 1.0f)
		{
			mViggnetTime = 1.0f;
		}

		// ビネットでフェードインする
		RenderCopyImage::GetInstance()->SetViggnetIndex(ExponentialInterpolation(10.0f, 0.0f, mViggnetTime, 1.0f));

		if (mViggnetTime == 1.0f) 
		{
			mIsFading = false;
		}
	}

	// シーン切り替え
	// Bボタンまたは、Enterキーでシーン遷移
	if (mInput->GetPused(Gamepad::Button::B) || mInput->GetTriggerKey(DIK_RETURN)) {
		sceneNo = SCENE::TITLE;
	}

}

void MenuScene::Draw() {
	
}

void MenuScene::DrawUI()
{
}
