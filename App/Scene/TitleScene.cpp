#include "TitleScene.h"
#include "GameEngine/Resource/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
#include "GameEngine/Editor/LevelEditor.h"
#include "GameEngine/Effect/Particle/ParticleManager.h"
#include "GameEngine/Effect/PostEffect/PostEffect.h"
#include "GameEngine/Editor/BlackBoard.h"
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
	mGroundObj->SetModelFullPath("MapObjects","ground.gltf");
	mGroundObj->SetScale(Vector3(1.0f, 1.0f, 1.0f));

	// 焚き火
	mWoodObj = std::make_unique<Object3d>();
	mWoodObj->Init("Wood");
	ModelManager::GetInstance()->LoadModel("Wood", "Wood.gltf");
	mWoodObj->SetModelFullPath("Wood","Wood.gltf");
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
	mPlayerObj->SetScale({ 2.0f,2.0f,2.0f });
	mPlayerObj->SetTranslate({ -1.0f,1.0f,-4.4f });
	// モデルを設定
	mPlayerObj->SetModelFullPath("Actor", "Actor.gltf");
	mPlayerObj->mSkinning = make_unique<Skinning>();
	mPlayerObj->mSkinning->Init("Actor", "Actor_Idle.gltf", mPlayerObj->GetModel()->mModelData);
	// モーションブレンド速度
	mPlayerObj->mSkinning->SetMotionBlendingInterval(2.0f);
	// アニメーション再生速度
	mPlayerObj->mSkinning->SetAnimationPlaySpeed(1.0f);
	// アニメーション登録
	mPlayerObj->mSkinning->CreateSkinningData("Actor", "Actor_Idle", ".gltf", mPlayerObj->GetModel()->mModelData, true);
	mPlayerObj->mSkinning->CreateSkinningData("Actor", "Actor_Run", ".gltf", mPlayerObj->GetModel()->mModelData, true);
	mPlayerObj->mSkinning->CreateSkinningData("Actor", "Actor_Jump", ".gltf", mPlayerObj->GetModel()->mModelData);
	

	// 剣
	mSwordObj = std::make_unique<Object3d>();
	mSwordObj->Init("Title Sword");
	ModelManager::GetInstance()->LoadModel("Weapons", "sword.gltf");
	mSwordObj->SetModelFullPath("weapons","sword.gltf");
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



	mTitleLogo = static_cast<FadeUI*>(UIManager::GetInstance()->GetUIPtr("TitleLogo"));
	// アルファ値を0(透明)に設定
	mTitleLogo->SetAlpha(0.0f);
	// 徐々に表示する
	mTitleLogo->StartFade(FadeIn);


	// スタート誘導UI (例:ボタンを押してください)
	UIManager::GetInstance()->CreateUI("PushStartUI", SceneName::Title);
	mPushStartUI = static_cast<FadeUI*>(UIManager::GetInstance()->GetUIPtr("PushStartUI"));
	// アルファ値を0(透明)に設定
	mPushStartUI->SetAlpha(0.0f);
	// 徐々に表示する
	mPushStartUI->StartFade(FadeIn);


	// UI - スタート誘導UI背景
	UIManager::GetInstance()->CreateUI("PushStartBackUI", SceneName::Title);
	mPushStartBackUI = static_cast<FadeUI*>(UIManager::GetInstance()->GetUIPtr("PushStartBackUI"));
	// アルファ値を0(透明)に設定
	mPushStartBackUI->SetAlpha(0.0f);
	// 徐々に表示する
	mPushStartBackUI->StartFade(FadeIn);


	// UI - ゲーム開始
	UIManager::GetInstance()->CreateUI("GameStartUI", SceneName::Title);
	mGameStartUI = static_cast<FadeUI*>(UIManager::GetInstance()->GetUIPtr("GameStartUI"));
	// アルファ値を0(透明)に設定
	mGameStartUI->SetAlpha(0.0f);
	// 非表示にしておく
	mGameStartUI->SetActive(false);


	// UI - システム(オプション)
	UIManager::GetInstance()->CreateUI("SystemUI", SceneName::Title);
	mSystemUI = static_cast<FadeUI*>(UIManager::GetInstance()->GetUIPtr("SystemUI"));
	// アルファ値を0(透明)に設定
	mSystemUI->SetAlpha(0.0f);
	// 非表示にしておく
	mSystemUI->SetActive(false);


	// UI - ゲーム終了
	UIManager::GetInstance()->CreateUI("QuitUI", SceneName::Title);
	mQuitUI = static_cast<FadeUI*>(UIManager::GetInstance()->GetUIPtr("QuitUI"));
	// アルファ値を0(透明)に設定
	mQuitUI->SetAlpha(0.0f);
	// 非表示にしておく
	mQuitUI->SetActive(false);

	// UI - 選択中のUI
	UIManager::GetInstance()->CreateUI("SelectingBackUI", SceneName::Title);
	mSelectingBackUI = static_cast<FadeUI*>(UIManager::GetInstance()->GetUIPtr("SelectingBackUI"));
	// アルファ値を0(透明)に設定
	mSelectingBackUI->SetAlpha(0.0f);
	// 非表示にしておく
	mSelectingBackUI->SetActive(false);

	// 選択中のUI番号
	mUISelectingNum = 0;


#pragma endregion

}

void TitleScene::Update() {

	// 右手のワールド行列を更新
	mWeaponParentMat = Multiply(
		mPlayerObj->mSkinning->GetBoneMatrix("mixamorig:RightHand"),
		mPlayerObj->GetWorldTransform()->GetWorldMatrix());


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
		ImGui::DragFloat3("FlamePos", &flamePos.x);
		ImGui::End();
#endif // _DEBUG

		// 発生座標の更新
		mFlameParticle->SetEmitterPos(mWoodObj->GetWorldTransform()->GetWorldPosition());
		mFlameParticle->Update();
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
			if (!mPushStartBackUI->GetIsFadeActive()) {
				// ループの実行
				mPushStartBackUI->StartFade(Loop, 0.5f);
			}

		}

		// ビネット係数の設定
		PostEffect::GetInstance()->SetViggnetIndex(
			ExponentialInterpolation(10.0f, 0.0f, viggnetOnlyTime, 1.0f));
	}
	// UIが非遷移中の場合
	else {

		// Bボタンを押したらゲーム選択画面に移行する
		// ボタンかキーボード入力でタイトルの選択受付に移行する
		if (InputManager::GetInstance()->GetPused(Gamepad::Button::B) ||
			InputManager::GetInstance()->GetTriggerKey(DIK_RETURN)) {

			// スタート誘導UIを非表示に
			mPushStartUI->StartFade(FadeOut, 0.5f);
			// 背景UIを非表示に
			mPushStartBackUI->StartFade(FadeOut, 0.5f);

			// UI表示切り替えフェーズに移行する
			mPhaseFunc = &TitleScene::UIFadePhase;
		}

	}

}

void TitleScene::GameSelectPhase() {

	// 選択画面のフェーズ

	// ゲーム開始/オプション/ゲーム終了の選択
	SelectInputProcess();


	// 選択しているUIへの処理を記載する
	switch (mUISelectingNum)
	{
	case 0: // ゲーム開始 

		// 選択中のUIの背景の座標を更新
		mSelectingBackUI->SetPos(mGameStartUI->GetPos());

		// Bボタン/Enterキー を押したらゲーム開始処理を行う
		if (InputManager::GetInstance()->GetPused(Gamepad::Button::B) ||
			InputManager::GetInstance()->GetTriggerKey(DIK_RETURN)) {


			// UIを透明にしていく

			// タイトルロゴ
			if (!mTitleLogo->GetIsFadeActive()) {
				mTitleLogo->StartFade(FadeOut, 0.5f);
			}
			// 選択中のUI背景
			if (!mSelectingBackUI->GetIsFadeActive()) {
				mSelectingBackUI->StartFade(FadeOut, 0.5f);
			}
			// スタート誘導UI
			if (!mGameStartUI->GetIsFadeActive()) {
				mGameStartUI->StartFade(FadeOut, 0.5f);
			}
			// オプションUI
			if (!mSystemUI->GetIsFadeActive()) {
				mSystemUI->StartFade(FadeOut, 0.5f);
			}
			// ゲーム終了UI
			if (!mQuitUI->GetIsFadeActive()) {
				mQuitUI->StartFade(FadeOut, 0.5f);
			}
			// UI背景も透明にする
			if (mSelectingBackUI->GetFadeStyle() == Loop) {
				mSelectingBackUI->StartFade(FadeOut, 0.5f);
			}

			// フェーズをUIフェードに移行
			mPhaseFunc = &TitleScene::UIFadePhase;

		}

		break;

	case 1: // システム(オプション) 

		// 選択中のUIの背景の座標を更新
		mSelectingBackUI->SetPos(mSystemUI->GetPos());

		// Bボタン/Enterキー を押したらゲーム開始処理を行う
		if (InputManager::GetInstance()->GetPused(Gamepad::Button::B) ||
			InputManager::GetInstance()->GetTriggerKey(DIK_RETURN)) {

			// メニューシーンの表示
			SceneManager::GetInstance()->MenuRequest();

		}

		break;

	case 2: // ゲーム終了 

		// 選択中のUIの背景の座標を更新
		mSelectingBackUI->SetPos(mQuitUI->GetPos());

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

}

void TitleScene::GameStartPhase() {

	// ゲーム開始前の演出処理
	Vector3 pos = mPlayerObj->GetWorldTransform()->GetWorldPosition();
	pos += {0.0f, 0.0f, BlackBoard::CombertGameFPS(6.0f)};

	if (pos.z >= 1.0f) {

		// プレイヤーをジャンプさせる
		if (mPlayerObj->mSkinning->GetNowSkinCluster()->name == "Actor_Run" &&
			!mPlayerObj->mSkinning->SearchToWaitingSkinCluster("Actor_Jump")) {
			mPlayerObj->mSkinning->SetNextAnimation("Actor_Jump");
		}

		if (mPlayerObj->mSkinning->GetNowSkinCluster()->name == "Actor_Jump") {

			pos.y += 0.1f;

			if (mPlayerObj->mSkinning->GetIsAnimationFinished("Actor_Jump")) {

				pos.y -= 0.2f;

				//mPlayerObj->mSkinning->SetNextAnimation("falling");
				mPlayerObj->mSkinning->SetAnimationPlaySpeed(2.5f);
				mPlayerObj->mSkinning->SetMotionBlendingInterval(4.0f);
			}
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

void TitleScene::UIFadePhase() {

	// 各UIのフェード処理を行う際に通過するフェーズ
	// UI表示の遷移はマネージャが一括で行うため
	// 更新処理以外のフェーズ移行やフラグの切り替えなどを行う
	switch (mSelectStep)
	{
	case SelectStep::START:

		// スタート誘導UIと背景UIが透明になったか確認する
		if (mPushStartUI->GetActive() != false &&
			mPushStartBackUI->GetActive() != false
			) {

			// 誘導及び背景のフェードアウトが終了したらそれぞれ非表示にしておく
			if (!mPushStartUI->GetIsFadeActive() &&
				!mPushStartBackUI->GetIsFadeActive()) {

				// 非表示処理
				mPushStartUI->SetActive(false);
				mPushStartBackUI->SetActive(false);

			}

			// 透明になっていない場合、
			// 早期リターンによりフェーズを終了する
			return;

		}

		// 透明になった場合、選択肢関係のUIのフェードを開始する
		
		// ゲーム開始UI
		if (!mGameStartUI->GetActive() &&
			!mGameStartUI->GetIsFadeActive()) {
			mGameStartUI->StartFade(FadeIn, 0.5f);
		}
		// システム移行UI
		if (!mSystemUI->GetActive() &&
			!mSystemUI->GetIsFadeActive()) {
			mSystemUI->StartFade(FadeIn, 0.5f);
		}

		// ゲーム終了UI
		if (!mQuitUI->GetActive() &&
			!mQuitUI->GetIsFadeActive()) {
			mQuitUI->StartFade(FadeIn, 0.5f);
		}

		// UI背景を点滅させる
		if (!mSelectingBackUI->GetActive()&&
			!mSelectingBackUI->GetIsFadeActive()) {
			// ループの実行
			mSelectingBackUI->StartFade(Loop, 0.5f);
		}

		// 選択肢関係のUIのフェードが終了したら
		// ゲーム選択画面に移行
		if (!mGameStartUI->GetIsFadeActive() && mGameStartUI->GetFadeStyle() == FadeIn &&
			!mSystemUI->GetIsFadeActive() && mSystemUI->GetFadeStyle() == FadeIn &&
			!mQuitUI->GetIsFadeActive() && mQuitUI->GetFadeStyle() == FadeIn
			) {

			// ゲーム選択フェーズに移行
			// フェーズ関数を変更
			mPhaseFunc = &TitleScene::GameSelectPhase;
			// 選択段階をゲーム選択に変更
			mSelectStep = SelectStep::GAMESELECT;
		}

		break;

	case SelectStep::GAMESELECT:

		// UI表示の遷移が終了したら
		if (mTitleLogo->GetActive() == false &&
			mSelectingBackUI->GetActive() == false &&
			mGameStartUI->GetActive() == false &&
			mSystemUI->GetActive() == false &&
			mQuitUI->GetActive() == false
			) {

			// ビネットをかける準備をする
			mGameStartVignnetingTime = 0.0f;
			// ゲーム開始演出に移行
			mPhaseFunc = &TitleScene::GameStartPhase;
			mSelectStep = SelectStep::GAMESTART;

			// プレイヤーのアニメーションを変更
			mPlayerObj->mSkinning->SetNextAnimation("Actor_Run");

		}

		break;
	case SelectStep::GAMESTART:




		if (0) {
			// ゲーム開始演出に移行
			mPhaseFunc = &TitleScene::GameStartPhase;
		}

		break;
	case SelectStep::UIFADE:
		break;
	case SelectStep::END:
		break;
	default:
		break;
	}

}

void TitleScene::SelectInputProcess() {

	// 選択肢を移動する
	// コントローラは 上下ボタン 及び スティック
	// 
	// キーボードは Wキー/Sキー 及び 上下キー

	// 上方向の入力
	if (InputManager::GetInstance()->GetPused(Gamepad::Button::UP) ||
		InputManager::GetInstance()->GetTriggerKey(DIK_W) ||
		InputManager::GetInstance()->GetTriggerKey(DIK_UP)) {

		// 上に移動
		if (mUISelectingNum > 0) {
			mUISelectingNum--;
		}
		else {
			mUISelectingNum = 2; // 最後の選択肢に戻る
		}
	}

	// 下方向の入力
	if (InputManager::GetInstance()->GetPused(Gamepad::Button::DOWN) ||
		InputManager::GetInstance()->GetTriggerKey(DIK_S) ||
		InputManager::GetInstance()->GetTriggerKey(DIK_DOWN)) {

		// 下に移動
		if (mUISelectingNum < 2) {
			mUISelectingNum++;
		}
		else {
			mUISelectingNum = 0; // 最初の選択肢に戻る
		}
	}

	// 決定ボタンの入力を確認
	// コントローラは Bボタン
	// キーボードは Enterキー

	if (InputManager::GetInstance()->GetPused(Gamepad::Button::B) ||
		InputManager::GetInstance()->GetTriggerKey(DIK_RETURN)) {

	}


}
