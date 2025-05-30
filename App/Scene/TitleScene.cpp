#include "TitleScene.h"
#include "GameEngine/Resource/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
#include "GameEngine/Editor/LevelEditor.h"
#include "GameEngine/Effect/Particle/ParticleManager.h"
#include "GameEngine/Effect/PostEffect/PostEffect.h"
#include "GameEngine/Editor/Framerate.h"
#include "GameEngine/Scene/SceneManager.h"

void TitleScene::Final() {

	// 登録したタイトルシーンのUIをリストから削除する
	UIManager::GetInstance()->DeleteSceneUI(SceneName::Title);

}

//　継承した関数
void TitleScene::Init() {

	// シーン名の設定
	mSceneName = "Title";
	// シーン内選択段階 (最初は遷移中にする)
	mSelectStep = SelectStep::SCENESTART;
	// 初期フェーズの設定
	mPhaseFunc = &TitleScene::StartPhase;


	// SkyBox
	TextureManager::GetInstance()->LoadTexture("skybox/skybox.dds");
	Skybox::GetInstance()->Init("skybox", "skybox.dds");

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

	// 焚き火
	mWoodObj = std::make_unique<Object3d>();
	mWoodObj->Init("Wood");
	ModelManager::GetInstance()->LoadModel("Wood", "Wood.gltf");
	mWoodObj->SetModel("Wood.gltf");
	mWoodObj->SetScale(Vector3(0.5f, 0.5f, 0.5f));
	mWoodObj->SetTranslate(Vector3(6.5f, 0.0f, 3.5f));

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
	mPlayerObj->SetTranslate({ -1.0f,1.0f,-4.4f });
	// モデルを設定
	mPlayerObj->SetModel("idle.gltf");
	mPlayerObj->mSkinning = make_unique<Skinning>();
	mPlayerObj->mSkinning->Init("player", "idleNB.gltf", mPlayerObj->GetModel()->mModelData);
	// モーションブレンド速度
	mPlayerObj->mSkinning->SetMotionBlendingInterval(2.0f);
	// アニメーション再生速度
	mPlayerObj->mSkinning->SetAnimationPlaySpeed(1.0f);
	// アニメーション登録
	mPlayerObj->mSkinning->CreateSkinningData("player", "idleNB", ".gltf", mPlayerObj->GetModel()->mModelData, true);
	mPlayerObj->mSkinning->CreateSkinningData("player", "run", ".gltf", mPlayerObj->GetModel()->mModelData, true);
	mPlayerObj->mSkinning->CreateSkinningData("player", "jump", ".gltf", mPlayerObj->GetModel()->mModelData);
	mPlayerObj->mSkinning->CreateSkinningData("player", "falling", ".gltf", mPlayerObj->GetModel()->mModelData, true);

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


	// 炎パーティクル
	mFlameParticle = std::make_unique<ParticleEmitter>("Flame");
	mFlameParticle->Init();
	ParticleManager::GetInstance()->CreateParticleGroupe(mFlameParticle->mName, "Flame.png");
	

#pragma region UI関連の初期化

	// タイトルロゴ (タイトル名を記載する)
	UIManager::GetInstance()->CreateUI("TitleLogo", SceneName::Title);
	mTitleLogo = dynamic_pointer_cast<FadeUI>(UIManager::GetInstance()->GetUIPtr("TitleLogo"));
	// アルファ値を0(透明)に設定
	mTitleLogo.lock()->SetAlpha(0.0f);
	// 徐々に表示する
	mTitleLogo.lock()->StartFade(FadeIn);


	// スタート誘導UI (例:ボタンを押してください)
	UIManager::GetInstance()->CreateUI("PushStartUI", SceneName::Title);
	mPushStartUI = dynamic_pointer_cast<FadeUI>(UIManager::GetInstance()->GetUIPtr("PushStartUI"));
	// アルファ値を0(透明)に設定
	mPushStartUI.lock()->SetAlpha(0.0f);
	// 徐々に表示する
	mPushStartUI.lock()->StartFade(FadeIn);


	// UI - スタート誘導UI背景
	UIManager::GetInstance()->CreateUI("PushStartBackUI", SceneName::Title);
	mPushStartBackUI = dynamic_pointer_cast<FadeUI>(UIManager::GetInstance()->GetUIPtr("PushStartBackUI"));
	// アルファ値を0(透明)に設定
	mPushStartBackUI.lock()->SetAlpha(0.0f);
	// 徐々に表示する
	mPushStartBackUI.lock()->StartFade(FadeIn);


	// UI - ゲーム開始
	UIManager::GetInstance()->CreateUI("GameStartUI", SceneName::Title);
	mGameStartUI = dynamic_pointer_cast<FadeUI>(UIManager::GetInstance()->GetUIPtr("GameStartUI"));
	// アルファ値を0(透明)に設定
	mGameStartUI.lock()->SetAlpha(0.0f);


	// UI - システム(オプション)
	UIManager::GetInstance()->CreateUI("SystemUI", SceneName::Title);
	mSystemUI = dynamic_pointer_cast<FadeUI>(UIManager::GetInstance()->GetUIPtr("SystemUI"));
	// アルファ値を0(透明)に設定
	mSystemUI.lock()->SetAlpha(0.0f);


	// UI - ゲーム終了
	UIManager::GetInstance()->CreateUI("QuitUI", SceneName::Title);
	mQuitUI = dynamic_pointer_cast<FadeUI>(UIManager::GetInstance()->GetUIPtr("QuitUI"));
	// アルファ値を0(透明)に設定
	mQuitUI.lock()->SetAlpha(0.0f);

	// UI - 選択中のUI
	UIManager::GetInstance()->CreateUI("SelectingBackUI", SceneName::Title);
	mSelectingBackUI = dynamic_pointer_cast<FadeUI>(UIManager::GetInstance()->GetUIPtr("SelectingBackUI"));
	// アルファ値を0(透明)に設定
	mSelectingBackUI.lock()->SetAlpha(0.0f);

	// 選択中のUI番号
	mUISelectingNum = 0;
	// 背景UIのカウントが上昇中であるか
	mIsUpperBackUICount = true;
	// UI表示の遷移中であるか
	mIsTransUI = false;
	// UI表示切替の進行度
	mUITransCount = 0.0f;

#pragma endregion

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
	mWoodObj->Update();
	mWoodObj->DrawGUI();
	mSwordObj->Update();
	mSwordObj->DrawGUI();
	mPlayerObj->Update();
	mPlayerObj->DrawGUI();

	// スカイボックス
	Skybox::GetInstance()->Update();
	Skybox::GetInstance()->DrawGUI("Skybox");

	// 関数ポインタを使用してフェーズごとに処理する
	(this->*mPhaseFunc)();

	if (mSelectStep != SelectStep::GAMESTART) {

		static Vector3 flamePos = Vector3(0.0f, 0.0f, -5.0f);

#ifdef _DEBUG
		ImGui::Begin("Flame");
		ImGui::DragFloat3("FlamePos" ,&flamePos.x);
		ImGui::End();
#endif // _DEBUG
		
		// 発生座標の更新
		mFlameParticle->SetEmitterPos(mWoodObj->GetWorldTransform()->GetWorldPosition());
		mFlameParticle->Update();
	}

	// シーン内の状態に応じて処理を変える
	switch (mSelectStep)
	{
	case SelectStep::SCENESTART:

		break;
	case SelectStep::START:

		break;
	case SelectStep::GAMESELECT:

		break;
	case SelectStep::GAMESTART:


		break;
	default:
		break;
	}

	// パーティクル 更新
	ParticleManager::GetInstance()->Update();

}

void TitleScene::Draw() {

	// Skybox 描画
	Skybox::GetInstance()->PreDraw();
	Skybox::GetInstance()->Draw();

	// 通常モデル 描画
	ModelManager::GetInstance()->PreDraw();
	mGroundObj->Draw();
	mWoodObj->Draw();
	mSwordObj->Draw();

	// アニメーションモデル 描画
	ModelManager::GetInstance()->PreDrawForSkinning();
	mPlayerObj->Draw();

	// パーティクル 描画
	ParticleManager::GetInstance()->PreDraw();
	ParticleManager::GetInstance()->Draw();

}

void TitleScene::DrawUI() {

}

void TitleScene::StartPhase() {

	// シーン開始時のビネット処理
	if (mIsTransitionForPreScene)
	{
		if (viggnetOnlyTime < 1.0f) {

			viggnetOnlyTime += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();

		}
		else if (viggnetOnlyTime >= 1.0f) {

			viggnetOnlyTime = 1.0f;

			// スタート入力誘導に移行
			mSelectStep = SelectStep::START;

			mIsTransitionForPreScene = false;

			// UI背景を点滅させる
			if (!mPushStartBackUI.lock()->GetIsFadeActive()) {
				// ループの実行
				mPushStartBackUI.lock()->StartFade(Loop, 0.5f);
			}

		}

		// ビネット係数の設定
		PostEffect::GetInstance()->SetViggnetIndex(
			ExponentialInterpolation(10.0f, 0.0f, viggnetOnlyTime, 1.0f));
	}
	// UIが非遷移中の場合
	if (!mIsTransUI) {

		// Bボタンを押したらゲーム選択画面に移行する
		// ボタンかキーボード入力でタイトルの選択受付に移行する
		if (InputManager::GetInstance()->GetPused(Gamepad::Button::B) ||
			InputManager::GetInstance()->GetTriggerKey(DIK_RETURN)) {

			// UI表示の遷移を行う
			mIsTransUI = true;

			// スタート誘導UIを非表示に
			mPushStartUI.lock()->StartFade(FadeOut, 0.5f);

		}

	}

	// UI表示の遷移中
	else if (mIsTransUI) {

		// 背景UIを非表示に
		if (mPushStartBackUI.lock()->GetFadeStyle() == Loop) {
			mPushStartBackUI.lock()->StartFade(FadeOut, 0.5f);
		}

		// 誘導UIと背景UIが透明になったか確認する
		if (mPushStartUI.lock()->GetIsFadeActive() == false &&
			mPushStartBackUI.lock()->GetIsFadeActive() == false
			) {

			// UI表示の遷移を終了
			mIsTransUI = false;
			// UI表示切替の進行度をリセット
			mUITransCount = 0.0f;
			// 選択画面に移行
			mPhaseFunc = &TitleScene::GameSelectPhase;

			// 各選択肢を徐々に表示する

			// ゲーム開始UI
			if (!mGameStartUI.lock()->GetIsFadeActive()) {
				mGameStartUI.lock()->StartFade(FadeIn, 0.5f);
			}
			// システム移行UI
			if (!mSystemUI.lock()->GetIsFadeActive()) {
				mSystemUI.lock()->StartFade(FadeIn, 0.5f);
			}

			// ゲーム終了UI
			if (!mQuitUI.lock()->GetIsFadeActive()) {
				mQuitUI.lock()->StartFade(FadeIn, 0.5f);
			}

		}

	}

}

void TitleScene::GameSelectPhase() {


	// UI表示の遷移していない場合
	if (!mIsTransUI) {

		// 上ボタン/Wキー を押したら選択UIを上に移動する
		if (InputManager::GetInstance()->GetPused(Gamepad::Button::UP) ||
			InputManager::GetInstance()->GetTriggerKey(DIK_W)) {
			// 選択UIを上に移動
			if (mUISelectingNum > 0) {
				mUISelectingNum--;
			}
			else {
				mUISelectingNum = 2; // 最後の選択肢に戻る
			}
		}

		// 下ボタン/Sキー を押したら選択UIを下に移動する
		else if (InputManager::GetInstance()->GetPused(Gamepad::Button::DOWN) ||
			InputManager::GetInstance()->GetTriggerKey(DIK_S)) {

			// 選択UIを下に移動
			if (mUISelectingNum < 2) {
				mUISelectingNum++;
			}
			else {
				mUISelectingNum = 0; // 最初の選択肢に戻る
			}
		}

		// あまり望ましくはないが、switchをネストし、選択しているUIへの処理を記載する
		switch (mUISelectingNum)
		{
		case 0: // ゲーム開始 

			// 選択中のUIの背景の座標を更新
			mSelectingBackUI.lock()->SetPos(mGameStartUI.lock()->GetPos());

			// Bボタン/Enterキー を押したらゲーム開始処理を行う
			if (InputManager::GetInstance()->GetPused(Gamepad::Button::B) ||
				InputManager::GetInstance()->GetTriggerKey(DIK_RETURN)) {

				// UI表示の遷移を行う
				mIsTransUI = true;
				// UI表示切替の進行度をリセット
				mUITransCount = 0.0f;

				// UIを透明にしていく

				// タイトルロゴもここで透明にする
				if (!mTitleLogo.lock()->GetIsFadeActive()) {
					mTitleLogo.lock()->StartFade(FadeOut, 0.5f);
				}

				if (!mSelectingBackUI.lock()->GetIsFadeActive()) {
					mSelectingBackUI.lock()->StartFade(FadeOut, 0.5f);
				}
				if (!mGameStartUI.lock()->GetIsFadeActive()) {
					mGameStartUI.lock()->StartFade(FadeOut, 0.5f);
				}
				if (!mSystemUI.lock()->GetIsFadeActive()) {
					mSystemUI.lock()->StartFade(FadeOut, 0.5f);
				}
				if (!mQuitUI.lock()->GetIsFadeActive()) {
					mQuitUI.lock()->StartFade(FadeOut, 0.5f);
				}


				// UI背景も透明にする
				if (mSelectingBackUI.lock()->GetFadeStyle() == Loop) {
					mSelectingBackUI.lock()->StartFade(FadeOut, 0.5f);
				}

			}

			break;

		case 1: // システム(オプション) 

			// 選択中のUIの背景の座標を更新
			mSelectingBackUI.lock()->SetPos(mSystemUI.lock()->GetPos());

			// Bボタン/Enterキー を押したらゲーム開始処理を行う
			if (InputManager::GetInstance()->GetPused(Gamepad::Button::B) ||
				InputManager::GetInstance()->GetTriggerKey(DIK_RETURN)) {

				// メニューシーンの表示
				SceneManager::GetInstance()->MenuRequest();

			}

			break;

		case 2: // ゲーム終了 

			// 選択中のUIの背景の座標を更新
			mSelectingBackUI.lock()->SetPos(mQuitUI.lock()->GetPos());

			// Bボタン/Enterキー を押したらゲーム終了処理を行う
			if (InputManager::GetInstance()->GetPused(Gamepad::Button::B) ||
				InputManager::GetInstance()->GetTriggerKey(DIK_RETURN)) {


				//
				// のちのち、ワンクッションの実装を行う
				//


				// シーンマネージャーに終了リクエストを送り、ゲームを終了する
				SceneManager::GetInstance()->EndRequest();
			}

			break;

		default:
			break;
		}

		// UI背景を点滅させる
		if (!mSelectingBackUI.lock()->GetIsFadeActive()) {
			// ループの実行
			mSelectingBackUI.lock()->StartFade(Loop, 0.5f);
		}

	}

	// UI表示の遷移中
	else if (mIsTransUI) {

		// UI表示の遷移が終了したら
		if (mTitleLogo.lock()->GetIsFadeActive() == false &&
			mSelectingBackUI.lock()->GetIsFadeActive() == false &&
			mGameStartUI.lock()->GetIsFadeActive() == false &&
			mSystemUI.lock()->GetIsFadeActive() == false &&
			mQuitUI.lock()->GetIsFadeActive() == false
			) {

			// UI表示の遷移を終了
			mIsTransUI = false;
			// UI表示切替の進行度をリセット
			mUITransCount = 0.0f;
			// ビネットをかける準備をする
			mGameStartVignnetingTime = 0.0f;
			// ゲーム開始演出に移行
			mPhaseFunc = &TitleScene::GameStartPhase;
			mSelectStep = SelectStep::GAMESTART;

			// プレイヤーのアニメーションを変更
			mPlayerObj->mSkinning->SetNextAnimation("run");

		}
	}

}

void TitleScene::GameStartPhase() {

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


}
