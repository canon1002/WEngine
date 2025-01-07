#include "TitleScene.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Editor/LevelEditor.h"
#include "App/Reaction/DamageReaction.h"
#include "GameEngine/Effect/Particle/ParticleManager.h"
#include "GameEngine/Object/Screen/RenderCopyImage.h"
#include "GameEngine/GameMaster/Framerate.h"
#include "GameEngine/Append/Collider/CollisionManager.h"

void TitleScene::Finalize() {}

//　継承した関数
void TitleScene::Init() {

	mObject = std::make_unique<ShadowObject>();
	mObject->Init("ShadowObject");
	ModelManager::GetInstance()->LoadModel("box", "box.gltf");
	mObject->SetModel("box.gltf");
	mObject->mModel->mTextureHandle = DirectXCommon::GetInstance()->mSrv->LoadTexture("uvChecker.dds");
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
	mTitleOne.sprite->SetTexture("pleaseButton.png");
	mTitleOne.sprite->SetPos({ 640.0f,500.0f});
	mTitleOne.sprite->SetScale({ 0.5f,0.5f});

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

	// -- エディタテスト -- //
	mBTNodeEditor = std::make_unique<BTNodeEditor>();
	mBTNodeEditor->Init();

	// -- OBB衝突テスト -- //

	// 衝突判定マネージャ
	mCollisionManager = std::make_unique<CollisionManager>();

	mOBBTestObj = std::make_unique<Object3d>();
	mOBBTestObj->Init("OBBTest");
	mOBBTestObj->SetModel("box.gltf");
	mOBBTestObj->mWorldTransform->translation = { 0.0f,7.0f,-85.0f };
	mOBBTestObj->mWorldTransform->scale = { 1.0f,1.0f,1.0f };
	mOBBTestObj->mWorldTransform->rotation = { 0.0f,0.0f,0.0f };

	mOBBTestObj->mCollider = new OBBCollider(mOBBTestObj->mWorldTransform, Vector3(1.0f, 1.0f, 1.0f));
	mOBBTestObj->mCollider->Init();
	mOBBTestObj->mCollider->SetAddTranslation(Vector3(0.0f, 0.55f, -0.1f));
	mOBBTestObj->mCollider->SetCollisionAttribute(kCollisionAttributePlayer);
	mOBBTestObj->mCollider->SetCollisionMask(kCollisionAttributeEnemyBullet);

	
	mOBBTestObj2 = std::make_unique<Object3d>();
	mOBBTestObj2->Init("OBBTest2");
	mOBBTestObj2->SetModel("box.gltf");
	mOBBTestObj2->mWorldTransform->translation = { -4.0f,7.0f,-85.0f };
	mOBBTestObj2->mWorldTransform->scale = { 1.0f,1.0f,1.0f };
	mOBBTestObj2->mWorldTransform->rotation = { 0.3f,0.0f,0.0f };

	mOBBTestObj2->mCollider = new OBBCollider(mOBBTestObj2->mWorldTransform, Vector3(1.0f, 1.0f, 1.0f));
	mOBBTestObj2->mCollider->Init();
	mOBBTestObj2->mCollider->SetAddTranslation(Vector3(0.0f, 0.55f, -0.1f));
	mOBBTestObj2->mCollider->SetCollisionAttribute(kCollisionAttributeEnemyBullet);
	mOBBTestObj2->mCollider->SetCollisionMask(kCollisionAttributePlayer);

}

void TitleScene::Update() {

	// オブジェクト更新
	mCamera->Update();
	mObject->Update();
	mGroundObj->Update();

	mBTNodeEditor->Update();

	mOBBTestObj->Update();
	mOBBTestObj->DrawGUI();
	mOBBTestObj2->Update();
	mOBBTestObj2->DrawGUI();

	// コライダーの更新処理
	mOBBTestObj->mCollider->Update();
	mOBBTestObj2->mCollider->Update();

	// コライダーリストへの追加処理
	mCollisionManager->SetCollider(mOBBTestObj->mCollider);
	mCollisionManager->SetCollider(mOBBTestObj2->mCollider);

	// 衝突判定を行う
	mCollisionManager->Update();

	// コライダーリストのクリア
	mCollisionManager->ClearColliders();

	// UI 更新
	mTitleOne.sprite->Update();
	mTitleLogo.sprite->Update();
	mTitleSelect.sprite->Update();

	mTitleOne.t += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
	if (mTitleOne.t >= 1.0f) {
		mTitleOne.t = 0.0f;
	}
	mTitleSelect.t += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
	if (mTitleSelect.t >= 1.0f) {
		mTitleSelect.t = 0.0f;
	}

	// カメラやシーン遷移をしていない場合に入力を受け付ける
	if (!mIsActiveTransition) {

		// ボタンかキーボード入力でタイトルの選択受付に移行する
		if (InputManager::GetInstance()->GetPused(Gamepad::Button::B) || InputManager::GetInstance()->GetTriggerKey(DIK_RETURN)) {

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
			viggnetOnlyTime += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();

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
			mCameraRot.t += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
		}
		else if (mCameraRot.t > 1.0f) {
			mCameraRot.t = 1.0f;
		}
		// 補間後の数値を計算
		mCamera->mWorldTransform->rotation = ExponentialInterpolation(mCameraRot.s, mCameraRot.e, mCameraRot.t, 1.0f);

		/// 平行移動

		// tを増加させ、座標を移動
		if (mCameraTrYZ.t < 1.0f) {
			mCameraTrYZ.t += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
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
			mCameraTrZ.t += (1.0f / Framerate::GetInstance()->GetFramerate());
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

	mOBBTestObj->Draw();
	mOBBTestObj2->Draw();

	mOBBTestObj->mCollider->Draw();
	mOBBTestObj2->mCollider->Draw();

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
