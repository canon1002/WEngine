#include "OverScene.h"
#include "GameEngine/Object/Screen/RenderCopyImage.h"
#include "GameEngine/GameMaster/Framerate.h"

void OverScene::Finalize(){

}

void OverScene::Init() {

	mIsFading = true;
	mViggnetTime = 0.0f;
}

void OverScene::Update() 
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
		PostEffect::GetInstance()->SetViggnetIndex(ExponentialInterpolation(10.0f, 0.0f, mViggnetTime, 1.0f));

		if (mViggnetTime == 1.0f) 
		{
			mIsFading = false;
		}
	}

	// シーン切り替え
	// Bボタンまたは、Enterキーでシーン遷移
	if (InputManager::GetInstance()->GetPused(Gamepad::Button::B) || InputManager::GetInstance()->GetTriggerKey(DIK_RETURN)) {
		sceneNo = SCENE::TITLE;
	}

}

void OverScene::Draw() {
	
}

void OverScene::DrawUI()
{
}
