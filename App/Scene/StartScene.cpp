#include "StartScene.h"
#include "GameEngine/Effect/PostEffect/PostEffect.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/GameMaster/Framerate.h"
#include "GameEngine/Scene/SceneManager.h"

void StartScene::Final() {

}

void StartScene::Init(){

	// ビネット初期設定(透明)
	PostEffect* render = PostEffect::GetInstance();
	render->SetViggnetColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	render->SetViggnetIndex(00.0f);
	render->SetViggnetMultiplier(10.0f);

	// 開始時の間
	mStartWaitTime = 0.0f;

	// ロゴ
	mLogoImage.sprite = std::make_unique<Sprite>();
	mLogoImage.sprite->Init();
	mLogoImage.sprite->SetTexture("Logo.png");
	mLogoImage.sprite->SetScale(Vector2(0.25f, 0.25f));
	mLogoImage.sprite->SetAnchorPoint(Vector2(0.5f, 0.5f));
	mLogoImage.sprite->SetPos(Vector2(640, 360));
	mLogoImage.sprite->SetColor(Color(1.0f, 1.0f, 1.0f, 0.0f));
	mLogoImage.isActive = false;

	// 背景
	mBackGround.sprite = std::make_unique<Sprite>();
	mBackGround.sprite->Init();
	mBackGround.sprite->SetTexture("BackGround.png");

	// フェード
	mIsFading = false;
	mViggnetTime = 0.0f;
}

void StartScene::Update()
{
	// 開始時に一時停止
	if (mStartWaitTime < kStartWaitTimeMax)
	{
		mStartWaitTime += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();

		if (mLogoImage.isActive) 
		{
			if (InputManager::GetInstance()->GetPused(Gamepad::Button::B) || InputManager::GetInstance()->GetTriggerKey(DIK_RETURN)) 
			{
				mStartWaitTime = kStartWaitTimeMax;
			}
		}

		if (mStartWaitTime >= kStartWaitTimeMax)
		{
			mStartWaitTime = kStartWaitTimeMax;

			// ロゴを表示
			mLogoImage.isActive = true;
		}
	}
	else
	{
		if (mLogoImage.isActive)
		{
			if (mLogoImage.t < 1.0f)
			{
				mLogoImage.t += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
				if (mLogoImage.t > 1.0f)
				{
					mLogoImage.t = 1.0f;
					mStartWaitTime = -1.0f;
				}

				// 透明度操作
				mLogoImage.sprite->SetColor(Color(1.0f, 1.0f, 1.0f, mLogoImage.t));
			}
			else if (mLogoImage.t < 2.0f)
			{
				mLogoImage.t += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
				mLogoImage.sprite->SetColor(Color(1.0f, 1.0f, 1.0f, (2.0f - mLogoImage.t)));

				if (mLogoImage.t >= 2.0f)
				{
					mIsFading = true;
				}

			}

		}
	}

	// フェード中処理
	if (mIsFading)
	{
		if (mViggnetTime < 1.0f)
		{
			mViggnetTime += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
		}
		else if (mViggnetTime >= 1.0f)
		{
			mViggnetTime = 1.0f;
			// タイトルシーンへ移行する
			SceneManager::GetInstance()->ChangeScene("Title");
		}

		// ビネットでフェードインする
		PostEffect::GetInstance()->SetViggnetIndex(ExponentialInterpolation(10.0f, 0.0f, mViggnetTime, 1.0f));

	}


	// シーン切り替え
	// Bボタンまたは、Enterキーでシーン遷移
	if (InputManager::GetInstance()->GetPused(Gamepad::Button::B) || InputManager::GetInstance()->GetTriggerKey(DIK_RETURN)) {
		//sceneNo = SCENE::TITLE;
	}

	mBackGround.sprite->Update();
	mLogoImage.sprite->Update();

}

void StartScene::Draw() {

}

void StartScene::DrawUI()
{
	// 画像 描画前処理
	SpriteAdministrator::GetInstance()->PreDraw();

	mBackGround.sprite->Draw();
	if (mLogoImage.isActive) {
		mLogoImage.sprite->Draw();
	}

}
