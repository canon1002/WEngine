#include "TitleScene.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Editor/LevelEditor.h"
#include "GameEngine/Effect/Particle/ParticleManager.h"
#include "GameEngine/Effect/PostEffect/PostEffect.h"
#include "GameEngine/GameMaster/Framerate.h"
#include "GameEngine/Scene/SceneManager.h"

void TitleScene::Final() {}

//　継承した関数
void TitleScene::Init() {

	// シーン内選択段階 (最初は遷移中にする)
	mSelectStep = SelectStep::SCENESTART;

	// カメラ設定
	mCamera = MainCamera::GetInstance();
	mCamera->Init();
	mCamera->mWorldTransform->rotation = { 0.35f ,0.3f,0.0f };
	mCamera->mWorldTransform->translation = { -15.0f,15.5f ,-30.0f };

	// 地面
	mGroundObj = std::make_unique<Object3d>();
	mGroundObj->Init("Title Ground");
	ModelManager::GetInstance()->LoadModel("MapObjects", "ground.gltf");
	mGroundObj->SetModel("ground.gltf");
	mGroundObj->SetScale(Vector3(1.0f, 1.0f, 1.0f));

	// スキニングアニメーションを生成
	ModelManager::GetInstance()->LoadModel("player", "idle.gltf");
	ModelManager::GetInstance()->LoadModel("player", "idleNB.gltf");
	ModelManager::GetInstance()->LoadModel("player", "run.gltf");
	ModelManager::GetInstance()->LoadModel("player", "jump.gltf");
	ModelManager::GetInstance()->LoadModel("player", "falling.gltf");

	// プレイヤーオブジェクト
	mPlayerObj = std::make_unique<Object3d>();
	mPlayerObj->Init("Title PlayerObj");
	mPlayerObj->SetScale({ 5.0f,5.0f,5.0f });
	mPlayerObj->SetTranslate({ 1.0f,1.0f,-4.4f });
	// モデルを設定
	mPlayerObj->SetModel("idle.gltf");
	mPlayerObj->mSkinning = make_unique<Skinning>();
	mPlayerObj->mSkinning->Init("player", "idleNB.gltf", mPlayerObj->GetModel()->modelData);
	// モーションブレンド速度
	mPlayerObj->mSkinning->SetMotionBlendingInterval(2.0f);
	// アニメーション再生速度
	mPlayerObj->mSkinning->SetAnimationPlaySpeed(1.0f);
	// アニメーション登録
	mPlayerObj->mSkinning->CreateSkinningData("player", "idleNB", ".gltf", mPlayerObj->GetModel()->modelData, true);
	mPlayerObj->mSkinning->CreateSkinningData("player", "run", ".gltf", mPlayerObj->GetModel()->modelData, true);
	mPlayerObj->mSkinning->CreateSkinningData("player", "jump", ".gltf", mPlayerObj->GetModel()->modelData);
	mPlayerObj->mSkinning->CreateSkinningData("player", "falling", ".gltf", mPlayerObj->GetModel()->modelData, true);

	// 剣
	mSwordObj = std::make_unique<Object3d>();
	mSwordObj->Init("Title Sword");
	ModelManager::GetInstance()->LoadModel("Weapons", "sword.gltf");
	mSwordObj->SetModel("sword.gltf");
	mSwordObj->mWorldTransform->scale = { 0.1f,0.1f,0.175f };
	mSwordObj->mWorldTransform->rotation = { 2.0f,-0.6f,1.4f };
	mSwordObj->mWorldTransform->translation = { 0.05f,0.0f,0.05f };

	// ペアレント
	mWeaponParentMat = MakeIdentity();
	mSwordObj->mWorldTransform->SetParent(mWeaponParentMat);
	
	// ビネットの調整
	mGameStartVignnetingTime = 0.0f;
	mIsTransitionForPreScene = true;
	viggnetOnlyTime = 0.0f;

	// ビネット初期設定(透明)
	PostEffect* postEfeect = PostEffect::GetInstance();
	postEfeect->SetViggnetColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	postEfeect->SetViggnetIndex(10.0f);
	postEfeect->SetViggnetMultiplier(10.0f);

#pragma region UI関連の初期化
	
	// タイトルロゴ (タイトル名を記載する)
	mTitleLogo.sprite = std::make_unique<Sprite>();
	mTitleLogo.sprite->Init();
	mTitleLogo.sprite->SetTexture("UI/System/Title.png");
	mTitleLogo.sprite->SetPos({ 640.0f,100.0f });
	mTitleLogo.sprite->SetAnchorPoint({ 0.5f,0.5f });
	mTitleLogo.t = 1.0f;
	mTitleLogo.displayCount = 0.0f;
	mTitleLogo.isActive = true;

	
	// スタート誘導UI (例:ボタンを押してください)
	mPushStartUI.sprite = std::make_unique<Sprite>();
	mPushStartUI.sprite->Init();
	mPushStartUI.sprite->SetTexture("UI/System/Push.png");
	mPushStartUI.sprite->SetPos({ 640.0f,500.0f });
	mPushStartUI.sprite->SetScale({ 0.5f,0.5f });
	mPushStartUI.sprite->SetAnchorPoint({ 0.5f,0.5f });
	mPushStartUI.t = 1.0f;
	mPushStartUI.displayCount = 0.5f;
	mPushStartUI.isActive = true;


	// UI - スタート誘導UI背景
	mPushStartBackUI.sprite = std::make_unique<Sprite>();
	mPushStartBackUI.sprite->Init();
	mPushStartBackUI.sprite->SetTexture("UI/System/BackLong.png");
	mPushStartBackUI.sprite->SetColor(Color(1.0f, 1.0f, 0.0f, 1.0f));
	mPushStartBackUI.sprite->SetPos({ 640.0f,500.0f });
	mPushStartBackUI.sprite->SetScale({ 0.5f,0.5f });
	mPushStartBackUI.sprite->SetAnchorPoint({ 0.5f,0.5f });
	mPushStartBackUI.t = 0.0f;
	mPushStartBackUI.displayCount = 0.5f;
	mPushStartBackUI.isActive = true;


	// UI - ゲーム開始
	mGameStartUI.sprite = std::make_unique<Sprite>();
	mGameStartUI.sprite->Init();
	mGameStartUI.sprite->SetTexture("UI/System/Start.png");
	mGameStartUI.sprite->SetPos({ 640.0f,460.0f });
	mGameStartUI.sprite->SetScale({ 0.5f,0.5f });
	mGameStartUI.sprite->SetAnchorPoint({ 0.5f,0.5f });
	mGameStartUI.t = 1.0f;
	mGameStartUI.displayCount = 0.5f;
	mGameStartUI.isActive = true;

	// UI - システム(オプション)
	mSystemUI.sprite = std::make_unique<Sprite>();
	mSystemUI.sprite->Init();
	mSystemUI.sprite->SetTexture("UI/System/System.png");
	mSystemUI.sprite->SetPos({ 640.0f,500.0f });
	mSystemUI.sprite->SetScale({ 0.5f,0.5f });
	mSystemUI.sprite->SetAnchorPoint({ 0.5f,0.5f });
	mSystemUI.t = 1.0f;
	mSystemUI.displayCount = 0.5f;
	mSystemUI.isActive = true;
	
	// UI - ゲーム終了
	mQuitUI.sprite = std::make_unique<Sprite>();
	mQuitUI.sprite->Init();
	mQuitUI.sprite->SetTexture("UI/System/Quit.png");
	mQuitUI.sprite->SetPos({ 640.0f,540.0f });
	mQuitUI.sprite->SetScale({ 0.5f,0.5f });
	mQuitUI.sprite->SetAnchorPoint({ 0.5f,0.5f });
	mQuitUI.t = 1.0f;
	mQuitUI.displayCount = 0.5f;
	mQuitUI.isActive = true;

	// UI - 選択中のUI
	mSelectingBackUI.sprite = std::make_unique<Sprite>();
	mSelectingBackUI.sprite->Init();
	mSelectingBackUI.sprite->SetTexture("UI/System/BackShort.png");
	mSelectingBackUI.sprite->SetColor(Color(1.0f, 1.0f, 0.0f, 1.0f));
	mSelectingBackUI.sprite->SetPos({ 640.0f,460.0f });
	mSelectingBackUI.sprite->SetScale({ 0.5f,0.5f });
	mSelectingBackUI.sprite->SetAnchorPoint({ 0.5f,0.5f });
	mSelectingBackUI.t = 0.0f;
	mSelectingBackUI.displayCount = 0.5f;
	mSelectingBackUI.isActive = true;

	// 選択中のUI番号
	mUISelectingNum = 0;
	// 背景UIのカウントが上昇中であるか
	mIsUpperBackUICount = true;
	// UI表示の遷移中であるか
	mIsTransUI = false;
	// UI表示切替の進行度
	mUITransCount = 0.0f;

#pragma endregion

	// パーティクルマネージャの初期化
	ParticleManager::GetInstance()->Init();

	// エミッター初期化
	mDashSmoke = std::make_unique<ParticleEmitter>("DashSmoke");
	mDashSmoke->Init();
	ParticleManager::GetInstance()->CreateParticleGroupe(mDashSmoke->mName, "circle.png");
		
	// -- エディタテスト -- //
	mBTNodeTestActor = std::make_unique<Actor>("NodeTester");
	mBTNodeEditor = std::make_unique<BTNodeEditor>(mBTNodeTestActor.get());
	mBTNodeEditor->Init();

	// SkyBox
	/*TextureManager::GetInstance()->LoadTexture("skybox/skybox.dds");
	mSkybox = Skybox::GetInstance();
	mSkybox->Init("skybox", "skybox.dds");*/

}

void TitleScene::Update() {

	// 右手のワールド行列を更新
	mWeaponParentMat = Multiply(
		mPlayerObj->mSkinning->GetSkeleton().joints[mPlayerObj->mSkinning->GetSkeleton().jointMap["mixamorig:RightHand"]
		].skeletonSpaceMatrix, mPlayerObj->GetWorldTransform()->GetWorldMatrix());

	// オブジェクト更新
	mCamera->Update();
	mGroundObj->Update();
	mGroundObj->DrawGUI();
	mSwordObj->Update();
	mSwordObj->DrawGUI();
	mPlayerObj->Update();
	mPlayerObj->DrawGUI();

	// スカイボックス
	//mSkybox->Update();

	mBTNodeEditor->Update();


	// タイトルロゴの更新
	mTitleLogo.sprite->Update();

	// シーン内の状態に応じて処理を変える
	switch (mSelectStep)
	{
	case SelectStep::SCENESTART:

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

				// スタート入力誘導に移行
				mSelectStep = SelectStep::START;
			}
			PostEffect::GetInstance()->SetViggnetIndex(ExponentialInterpolation(10.0f, 0.0f, viggnetOnlyTime, 1.0f));
		}

		break;
	case SelectStep::START:
		
		// スタート誘導UI/UI背景を更新
		mPushStartUI.sprite->Update();
		mPushStartBackUI.sprite->Update();

		// UIが非遷移中の場合
		if(!mIsTransUI){

			// Bボタンを押したらゲーム選択画面に移行する
			// ボタンかキーボード入力でタイトルの選択受付に移行する
			if (InputManager::GetInstance()->GetPused(Gamepad::Button::B) ||
				InputManager::GetInstance()->GetTriggerKey(DIK_RETURN)) {

				// UI表示の遷移を行う
				mIsTransUI = true;
				// UI表示切替の進行度をリセット
				mUITransCount = 0.0f;
			}

			// UI背景を点滅させる
			if (mIsUpperBackUICount) {
				// 透明度を増加させる
				mPushStartBackUI.t += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();

				// 不透明になるギリギリで変更
				if (mPushStartBackUI.t >= 0.9f) {
					mIsUpperBackUICount = false;
				}

			}
			else {
				// 透明度を減少させる
				mPushStartBackUI.t -= (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
				
				// 透明になるギリギリで変更
				if (mPushStartBackUI.t <= 0.1f) {
					mIsUpperBackUICount = true;
				}
			}

			// 透明度の変更
			mPushStartBackUI.sprite->SetAlpha(mPushStartBackUI.t);

		}
		// UI表示の遷移中
		else if (mIsTransUI) {
			// UIを透明にしていく
			if (mUITransCount < 1.0f) {
				mUITransCount += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
				mPushStartUI.sprite->SetAlpha(1.0f - mUITransCount);
				mPushStartBackUI.sprite->SetAlpha(1.0f - mUITransCount);
			}
			// UI表示の遷移が終了したら
			else if (mUITransCount >= 1.0f) {

				// UI表示の遷移を終了
				mIsTransUI = false;
				// UI表示切替の進行度をリセット
				mUITransCount = 0.0f;
				// 背景UIの透明度を上げるようにする
				mIsUpperBackUICount = true;
				// 選択画面に移行
				mSelectStep = SelectStep::GAMESELECT;
			}
		}

		break;
	case SelectStep::GAMESELECT:

		// UI表示の遷移していない場合
		if (!mIsTransUI) {

			// 開始/システム/終了 のUIをそれぞれ不透明にしていく
			if (mUITransCount < 1.0f) {
				mUITransCount += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
			}
			else if (mUITransCount > 1.0f) {
				mUITransCount = 1.0f;
			}

			// それぞれ透明度を調整
			mGameStartUI.sprite->SetColor(Color(1.0f, 1.0f, 1.0f, mUITransCount));
			mSystemUI.sprite->SetColor(Color(1.0f, 1.0f, 1.0f, mUITransCount));
			mQuitUI.sprite->SetColor(Color(1.0f, 1.0f, 1.0f, mUITransCount));


			// あまり望ましくはないが、switchをネストし、選択しているUIへの処理を記載する
			switch (mUISelectingNum)
			{
			case 0: // ゲーム開始 

				// Bボタンを押したらゲーム開始処理を行う
				if (InputManager::GetInstance()->GetPused(Gamepad::Button::B) ||
					InputManager::GetInstance()->GetTriggerKey(DIK_RETURN)) {

					// UI表示の遷移を行う
					mIsTransUI = true;
					// UI表示切替の進行度をリセット
					mUITransCount = 0.0f;
				}

				break;

			case 1: // システム(オプション) 
				
				//
				// オプション画面や処理ができていないため実装待ち
				//
				
				break;

			case 2: // ゲーム終了 
				
				//
				// ゲームシーンに終了をさせる処理ができていないため実装待ち
				//

				break;

			default:
				break;
			}

			// UI背景を点滅させる
			if (mIsUpperBackUICount) {
				// 透明度を増加させる
				mSelectingBackUI.t += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();

				// 不透明になるギリギリで変更
				if (mSelectingBackUI.t >= 0.9f) {
					mIsUpperBackUICount = false;
				}

			}
			else {
				// 透明度を減少させる
				mSelectingBackUI.t -= (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();

				// 透明になるギリギリで変更
				if (mSelectingBackUI.t <= 0.1f) {
					mIsUpperBackUICount = true;
				}
			}

			// 透明度の変更
			mSelectingBackUI.sprite->SetAlpha(mSelectingBackUI.t);

		}

		// UI表示の遷移中
		if (mIsTransUI) {

			// UIを透明にしていく
			if (mUITransCount < 1.0f) {
				mUITransCount += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
				
				// タイトルロゴもここで透明にする
				mSelectingBackUI.sprite->SetAlpha(1.0f - mUITransCount);
				mTitleLogo.sprite->SetAlpha(1.0f - mUITransCount);
				mGameStartUI.sprite->SetAlpha(1.0f - mUITransCount);
				mSystemUI.sprite->SetAlpha(1.0f - mUITransCount);
				mQuitUI.sprite->SetAlpha(1.0f - mUITransCount);
			}

			// UI表示の遷移が終了したら
			else if (mUITransCount >= 1.0f) {

				// UI表示の遷移を終了
				mIsTransUI = false;
				// UI表示切替の進行度をリセット
				mUITransCount = 0.0f;
				// ビネットをかける準備をする
				mGameStartVignnetingTime = 0.0f;
				// ゲーム開始演出に移行
				mSelectStep = SelectStep::GAMESTART;
				// プレイヤーのアニメーションを変更
				mPlayerObj->mSkinning->SetNextAnimation("run");

			}
		}

		// 開始/システム/終了 のUIをそれぞれ更新
		mGameStartUI.sprite->Update();
		mSystemUI.sprite->Update();
		mQuitUI.sprite->Update();
		mSelectingBackUI.sprite->Update();

		break;
	
	case SelectStep::GAMESTART:

		// ゲーム開始前の演出処理
		Vector3 pos = mPlayerObj->GetWorldTransform()->GetWorldPosition();
		pos += {0.0f, 0.0f, 0.1f};
		
		if (pos.z >= 1.0f) {
			// プレイヤーをジャンプさせる
			if (mPlayerObj->mSkinning->GetNowSkinCluster()->name == "run" &&
				!mPlayerObj->mSkinning->SearchToWaitingSkinCluster("jump")) {
				mPlayerObj->mSkinning->SetNextAnimation("jump");
			}

			if (mPlayerObj->mSkinning->GetNowSkinCluster()->name == "jump") {

				pos.y += 0.25f;

				if (mPlayerObj->mSkinning->GetIsAnimationFinished("jump")) {

					mPlayerObj->mSkinning->SetNextAnimation("falling");
					mPlayerObj->mSkinning->SetAnimationPlaySpeed(2.5f);
					mPlayerObj->mSkinning->SetMotionBlendingInterval(4.0f);
				}
			}
			else {
				pos.y -= 0.2f;
			}
			
		}
		if (pos.z >= 8.5f) {
			pos.y -= 0.2f;

			// ゲームシーンへの遷移処理
			if (mGameStartVignnetingTime < 1.0f) {
				mGameStartVignnetingTime += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
			}

			// 画面をどんどん暗くする
			PostEffect::GetInstance()->SetViggnetIndex(ExponentialInterpolation(0.0f, 10.0f, mGameStartVignnetingTime, 1.0f));

			// 遷移が終了したら
			if (mGameStartVignnetingTime >= 1.0f) {
				// ゲームシーンへ移行する
				SceneManager::GetInstance()->ChangeScene("Game");
			}

		}

		mPlayerObj->SetTranslate(pos);

		break;
	default:
		break;
	}

	// プレイヤーが走っている場合
	if (mPlayerObj->mSkinning->GetNowSkinCluster()->name == "run") {
		// 発生座標の更新
		mDashSmoke->SetEmitterPos(mPlayerObj->GetWorldTransform()->GetWorldPosition());
		// ダッシュ煙を発生させる
		mDashSmoke->Update();
	}

	// パーティクル 更新
	ParticleManager::GetInstance()->Update();

}

void TitleScene::Draw() {

	//// Skybox 描画
	//mSkybox->PreDraw();
	//mSkybox->Draw();

	// 通常モデル 描画
	ModelManager::GetInstance()->PreDraw();
	mGroundObj->Draw();
	mSwordObj->Draw();

	// アニメーションモデル 描画
	ModelManager::GetInstance()->PreDrawForSkinning();
	mPlayerObj->Draw();

	// パーティクル 描画
	ParticleManager::GetInstance()->PreDraw();
	ParticleManager::GetInstance()->Draw();

}

void TitleScene::DrawUI()
{
	// 画像 描画前処理
	SpriteAdministrator::GetInstance()->PreDraw();

	// タイトルロゴ
	mTitleLogo.sprite->Draw();

	// シーン内の状態に応じて処理を変える
	switch (mSelectStep)
	{
	case SelectStep::SCENESTART:
		
		break;
	case SelectStep::START:

		mPushStartBackUI.sprite->Draw();
		mPushStartUI.sprite->Draw();

		break;
	case SelectStep::GAMESELECT:

		// 選択中のUI座標に表示
		mSelectingBackUI.sprite->Draw();

		mGameStartUI.sprite->Draw();
		//mSystemUI.sprite->Draw();
		//mQuitUI.sprite->Draw();

		break;
	case SelectStep::GAMESTART:


		break;
	default:
		break;
	}

}
