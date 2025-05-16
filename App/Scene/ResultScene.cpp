#include "ResultScene.h"
#include "GameEngine/Effect/PostEffect/PostEffect.h"
#include "GameEngine/Editor/Framerate.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Scene/SceneManager.h"

void ResultScene::Final() {

}

void ResultScene::Init() {

	mTitleOne.sprite = std::make_unique<Sprite>();
	mTitleOne.sprite->Init();
	mTitleOne.sprite->SetTexture("UI/System/Push.png");
	mTitleOne.sprite->SetPos({ 640.0f,500.0f });
	mTitleOne.sprite->SetScale({ 0.5f,0.5f });

	mTitleOne.sprite->SetAnchorPoint({ 0.5f,0.5f });
	mTitleOne.t = 0.0f;
	mTitleOne.displayCount = 0.5f;
	mTitleOne.isActive = true;

	mTitleLogo.sprite = std::make_unique<Sprite>();
	mTitleLogo.sprite->Init();
	mTitleLogo.sprite->SetTexture("UI/System/Win.png");
	mTitleLogo.sprite->SetPos({ 640.0f,160.0f });
	mTitleLogo.sprite->SetScale({ 0.8f,0.8f });

	mTitleLogo.sprite->SetAnchorPoint({ 0.5f,0.5f });
	mTitleLogo.t = 0.0f;
	mTitleLogo.displayCount = 0.0f;
	mTitleLogo.isActive = true;

	mIsFading = true;
	mViggnetTime = 0.0f;
	mIsFadingSceneEnd = false;
}

void ResultScene::Update()
{
	// UI 更新
	mTitleOne.sprite->Update();
	mTitleLogo.sprite->Update();


	if (mIsFading)
	{
		if (mViggnetTime < 1.0f)
		{
			mViggnetTime += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
		}
		else if (mViggnetTime >= 1.0f)
		{
			mViggnetTime = 1.0f;
			mIsFadingSceneEnd = true;
		}

		// ビネットでフェードインする
		PostEffect::GetInstance()->SetViggnetIndex(ExponentialInterpolation(10.0f, 0.0f, mViggnetTime, 1.0f));

		if (mViggnetTime == 1.0f)
		{
			mIsFading = false;
		}
	}

	else {
		mTitleOne.t += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
		if (mTitleOne.t >= 1.0f) {
			mTitleOne.t = 0.0f;
		}
	}

	if (mIsFadingSceneEnd)
	{
		if (mViggnetTime > 1.0f)
		{
			mViggnetTime -= (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
		}
		else if (mViggnetTime <= 1.0f)
		{
			mViggnetTime = 1.0f;
			mIsFadingSceneEnd = true;
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
		// タイトルシーンへ移行する
		SceneManager::GetInstance()->ChangeScene("Title");
	}

}

void ResultScene::Draw() {

}

void ResultScene::DrawUI()
{

	// 画像 描画前処理
	SpriteAdministrator::GetInstance()->PreDraw();

	mTitleLogo.sprite->Draw();

	// カメラやシーン遷移をしていない場合にのみUIを表示
	if (!mIsFading) {
		
		if (mTitleOne.t <= mTitleOne.displayCount) {
			mTitleOne.sprite->Draw();
		}

	}

}
