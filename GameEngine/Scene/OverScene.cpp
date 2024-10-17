#include "OverScene.h"
#include "GameEngine/Object/Screen/RenderCopyImage.h"

void OverScene::Finalize(){

}

void OverScene::Init() {
	// 入力を取得する
	mInput = InputManager::GetInstance();

	mIsFading = true;
	mViggnetTime = 0.0f;
}

void OverScene::Update() 
{
	if (mIsFading)
	{
		if (mViggnetTime < 1.0f)
		{
			mViggnetTime += 1.0f / 60.0f;
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

void OverScene::Draw() {
	
}

void OverScene::DrawUI()
{
}
