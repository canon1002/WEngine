#include "TitleScene.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Editor/LevelEditor.h"
#include "App/Reaction/DamageReaction.h"
#include "GameEngine/Effect/Particle/ParticleManager.h"
#include "GameEngine/Object/Screen/RenderCopyImage.h"

void TitleScene::Finalize() {}

//　継承した関数
void TitleScene::Init() {
	// 入力を取得する
	mInput = InputManager::GetInstance();

	mObject = std::make_unique<ShadowObject>();
	mObject->Init("ShadowObject");
	ModelManager::GetInstance()->LoadModel("box", "box.gltf");
	mObject->SetModel("box.gltf");
	mObject->mModel->mTextureHandle = DirectXCommon::GetInstance()->srv_->LoadTexture("uvChecker.dds");
	mObject->mWorldTransform->rotation = { 0.5f,0.0f,0.0f };

	// カメラ設定
	mCamera = MainCamera::GetInstance();
	mCamera->Initialize();
	mCamera->mWorldTransform->rotation = { 0.524f ,0.0f,0.0f };
	mCamera->mWorldTransform->translation = { 0.0f,80.0f ,-160.0f };

	// 地面
	mGroundObj = std::make_unique<Object3d>();
	mGroundObj->Init("Title Ground");
	ModelManager::GetInstance()->LoadModel("MapObjects", "Plane.gltf");
	mGroundObj->SetModel("Plane.gltf");
	mGroundObj->SetScale(Vector3(1024.0f, 1.0f, 1024.0f));

	// 建物
	mBuildingObj = std::make_unique<Object3d>();
	mBuildingObj->Init("Title Building");
	ModelManager::GetInstance()->LoadModel("MapObjects", "Wall.gltf");
	mBuildingObj->SetModel("Wall.gltf");
	mBuildingObj->SetScale(Vector3(1.0f, 1.0f, 1.0f));

	/// スタート補間

	// カメラ回転
	mCameraRot.s = { mCamera->mWorldTransform->rotation.x,0.0f,0.0f };
	mCameraRot.e = { 0.05f,0.0f,0.0f };
	mCameraRot.t = 0.0f;

	// カメラ移動
	mCameraTrYZ.s = mCamera->mWorldTransform->translation;
	mCameraTrYZ.e = { 0.0f,6.0f,-100.0f };
	mCameraTrYZ.t = 0.0f;

	// カメラ移動(ゲームシーン移行)
	mCameraTrZ.s = mCameraTrYZ.e;
	mCameraTrZ.e = { 0.0f,6.0f,-60.0f };
	mCameraTrZ.t = 0.0f;

	
	mIsTransitionForPreScene = true;
	viggnetOnlyTime = 0.0f;
	mIsTransitionTitleSelect = false;
	mIsTransitionGameScene = false;
	mIsActiveTransition = false;

	// ビネット初期設定(透明)
	RenderCopyImage* render = RenderCopyImage::GetInstance();
	render->SetViggnetColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	render->SetViggnetIndex(10.0f);
	render->SetViggnetMultiplier(10.0f);

	// 以下 UI
	mTitleOne.sprite = std::make_unique<Sprite>();
	mTitleOne.sprite->Init();
	mTitleOne.sprite->SetTexture("UI/UI2.png");
	mTitleOne.sprite->SetPos({ 640.0f,500.0f});
	mTitleOne.sprite->SetAnchorPoint({ 0.5f,0.5f });
	mTitleOne.t = 0.0f;
	mTitleOne.displayCount = 0.5f;
	mTitleOne.isActive = true;
	
	// ゲームタイトル
	mTitleLogo.sprite = std::make_unique<Sprite>();
	mTitleLogo.sprite->Init();
	mTitleLogo.sprite->SetTexture("UI/Title.png");
	mTitleLogo.sprite->SetPos({ 640.0f,100.0f});
	mTitleLogo.sprite->SetAnchorPoint({ 0.5f,0.5f });
	mTitleLogo.t = 0.0f;
	mTitleLogo.displayCount = 0.0f;
	mTitleLogo.isActive = true;
	
	// 以下 UI
	mTitleSelect.sprite = std::make_unique<Sprite>();
	mTitleSelect.sprite->Init();
	mTitleSelect.sprite->SetTexture("UI/UI3.png");
	mTitleSelect.sprite->SetPos({ 640.0f,500.0f});
	mTitleSelect.sprite->SetAnchorPoint({ 0.5f,0.5f });
	mTitleSelect.t = 0.0f;
	mTitleSelect.displayCount = 0.5f;
	mTitleSelect.isActive = true;


}

void TitleScene::Update() {

	// オブジェクト更新
	mCamera->Update();
	mObject->Update();
	mGroundObj->Update();
	mBuildingObj->Update();
	mBuildingObj->DrawGUI();

	// UI 更新
	mTitleOne.sprite->Update();
	mTitleLogo.sprite->Update();
	mTitleSelect.sprite->Update();

	mTitleOne.t += 1.0f / 60.0f;
	if (mTitleOne.t >= 1.0f) {
		mTitleOne.t = 0.0f;
	}
	mTitleSelect.t += 1.0f / 60.0f;
	if (mTitleSelect.t >= 1.0f) {
		mTitleSelect.t = 0.0f;
	}

	// カメラやシーン遷移をしていない場合に入力を受け付ける
	if (!mIsActiveTransition) {

		// ボタンかキーボード入力でタイトルの選択受付に移行する
		if (mInput->GetPused(Gamepad::Button::B) || mInput->GetTriggerKey(DIK_RETURN)) {

			// タイトルの選択画面に移行する
			if (!mIsTransitionTitleSelect) {
				mIsTransitionTitleSelect = true;
				mIsActiveTransition = true;
			}

			// 選択画面でゲーム開始を選択したら遷移後にシーン移行
			else if (!mIsTransitionGameScene) {
				mIsTransitionGameScene = true;
				mIsActiveTransition = true;

				// 遷移のスタート地点設定
				mCameraTrZ.s = mCamera->mWorldTransform->translation;
			}

		}
	}

	if (mIsTransitionForPreScene) 
	{
		if (viggnetOnlyTime < 1.0f) 
		{
			viggnetOnlyTime += (1.0f / 60.0f);

		}
		else if (viggnetOnlyTime >= 1.0f) 
		{
			viggnetOnlyTime = 1.0f;
			mIsActiveTransition = false;
		}
		RenderCopyImage::GetInstance()->SetViggnetIndex(ExponentialInterpolation(10.0f, 0.0f, viggnetOnlyTime, 1.0f));
	}

	// カメラ遷移
	if (mIsTransitionTitleSelect && !mIsTransitionGameScene) {

		/// 回転

		// tを増加させ、座標を移動
		if (mCameraRot.t < 1.0f) {
			mCameraRot.t += 1.0f / 60.0f;
		}
		else if (mCameraRot.t > 1.0f) {
			mCameraRot.t = 1.0f;
		}
		// 補間後の数値を計算
		mCamera->mWorldTransform->rotation = ExponentialInterpolation(mCameraRot.s, mCameraRot.e, mCameraRot.t, 1.0f);

		/// 平行移動

		// tを増加させ、座標を移動
		if (mCameraTrYZ.t < 1.0f) {
			mCameraTrYZ.t += 1.0f / 60.0f;
		}
		else if (mCameraTrYZ.t > 1.0f) {
			mCameraTrYZ.t = 1.0f;
		}
		// 補間後の数値を計算
		mCamera->mWorldTransform->translation = ExponentialInterpolation(mCameraTrYZ.s, mCameraTrYZ.e, mCameraTrYZ.t, 1.0f);

		if (mCameraRot.t == 1.0f && mCameraTrYZ.t == 1.0f) {
			mIsActiveTransition = false;
		}

	}

	// ゲームシーン遷移
	if (mIsTransitionGameScene) {

		/// 平行移動

		// tを増加させ、座標を移動
		if (mCameraTrZ.t < 1.0f) {
			mCameraTrZ.t += 1.0f / 60.0f;
		}
		else if (mCameraTrZ.t > 1.0f) {
			mCameraTrZ.t = 1.0f;
		}
		// 補間後の数値を計算
		mCamera->mWorldTransform->translation = ExponentialInterpolation(mCameraTrZ.s, mCameraTrZ.e, mCameraTrZ.t, 1.0f);
		// 画面をどんどん暗くする
		RenderCopyImage::GetInstance()->SetViggnetIndex(ExponentialInterpolation(0.0f, 10.0f, mCameraTrZ.t, 1.0f));

		if (mCameraTrZ.t == 1.0f) {
			mIsActiveTransition = false;

			// 遷移が終了したら
			if (mIsActiveTransition == false) {
				// ゲームシーンへ移行する
				sceneNo = SCENE::STAGE;
			}
		}

		
	}

}

void TitleScene::Draw() {

	ModelManager::GetInstance()->PreDrawForShadow();
	mObject->Draw();
	mGroundObj->Draw();
	mBuildingObj->Draw();
}

void TitleScene::DrawUI()
{

	// 画像 描画前処理
	SpriteAdministrator::GetInstance()->PreDraw();

	// カメラやシーン遷移をしていない場合にのみUIを表示
	if (!mIsActiveTransition) {
		if (!mIsTransitionTitleSelect) {
			if (mTitleOne.t <= mTitleOne.displayCount) {
				mTitleOne.sprite->Draw();
			}
		}

		else if (!mIsTransitionGameScene) {
			mTitleLogo.sprite->Draw();
			if (mTitleSelect.t <= mTitleSelect.displayCount) {
				mTitleSelect.sprite->Draw();
			}
		}
	}

}
