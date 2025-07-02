#include "GameScene.h"
#include "GameEngine/Object/Sprite/SpriteAdministrator.h"
#include <cassert>
#include <fstream>
#include "GameEngine/Resource/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
#include "GameEngine/Editor/LevelEditor.h"
#include "GameEngine/Effect/Particle/ParticleManager.h"
#include "GameEngine/Effect/PostEffect/PostEffect.h"
#include "GameEngine/Editor/Framerate.h"
#include "GameEngine/Scene/SceneManager.h"
#include "App/Manager/GameManager.h"

void GameScene::Final() {

	// ゲームマネージャ 終了処理
	GameManager::GetInstance()->Final();
}

void GameScene::Init() {

	// シーン名の設定
	mSceneName = "Game";

	// ゲームマネージャ宣言/初期化
	GameManager* gameManager = GameManager::GetInstance();
	gameManager->Init();

	// オブジェクト/リソースの初期化
	this->InitObject();
	// Actor及び関係箇所の初期化
	this->InitActor();
	// UIの初期化
	this->InitUI();
	// カメラの初期化
	this->InitCamera();
	// エフェクト/パーティクルの初期化
	this->InitEffects();

	// アクターの初期化終了後、マネージャに参照を渡す
	gameManager->SetPlayer(mPlayer.get()); // プレイヤーを参照
	gameManager->SetBossEnemy(mBoss.get()); // ボス敵を参照

	// ゲームシーンの段階
	mPhase = Phase::BEGIN;
	mPhaseFunc = &GameScene::BeginPhase;

}

void GameScene::Update() {

	// ボタン入力 & 行動表示
	for (int32_t i = 0; i < mButtonUI.size(); i++) {
		mButtonUI[i]->Update();
		mActionsUI[i]->Update();
	}

	// -- デバッグ・開発用操作 -- //
#ifdef _DEBUG

	// 1キーを押したうえで
	if (InputManager::GetInstance()->GetPushKey(DIK_1)) {

		// Enterキーでシーン遷移
		if (InputManager::GetInstance()->GetTriggerKey(DIK_RETURN)) {
			// リザルトシーンへ移行する
			SceneManager::GetInstance()->ChangeScene("Result");
		}

		// Rキーでリセット
		if (InputManager::GetInstance()->GetPushKey(DIK_R)) {
			Init();
		}

	}

	// ImGuiの表示
	Skybox::GetInstance()->DrawGUI("Skybox");
	mPlayer->DrawGUI();
	mBoss->DrawGUI();

#endif // _DEBUG

	// 各フェーズごとの処理を関数ポインタで実行する
	(this->*mPhaseFunc)();

	// フェーズ切り替え処理
	switch (mPhase)
	{
	case Phase::BEGIN:
		mPhaseFunc = &GameScene::BeginPhase;
		break;
	case Phase::BATTLE:
		mPhaseFunc = &GameScene::BattlePhase;
		break;
	case Phase::LOSE:
		mPhaseFunc = &GameScene::LosePhase;
		break;
	case Phase::WIN:
		mPhaseFunc = &GameScene::WinPhase;
		break;
	default:
		break;
	}

	// -- 全フェーズ共通の更新処理 -- //

	// カメラ
	MainCamera::GetInstance()->Update();
	// スカイボックス
	Skybox::GetInstance()->Update();
	// レベルエディタ
	LevelEditor::GetInstance()->Update();
	// パーティクル
	ParticleManager::GetInstance()->Update();

}


void GameScene::BeginPhase() {

	// カメラ操作強制OFF
	MainCamera::GetInstance()->SetCameraRotateControll(false);

	// ビネットを調整してフェードイン
	if (mViggnetTime < 1.0f) {
		// ビネットの時間を進める
		mViggnetTime += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
		// ビネットの時間が1.0fを超えたら1.0fに設定
		if (mViggnetTime >= 1.0f) {
			mViggnetTime = 1.0f;
		}
		// ビネットの操作
		PostEffect::GetInstance()->SetViggnetIndex(ExponentialInterpolation(10.0f, 0.0f, mViggnetTime, 1.0f));
	}
	// フェードイン・プレイヤーの移動 終了後
	else if (mPlayer->GetWorldPos().y <= 0.5f) {

		// カメラを徐々に回転させる
		if (MainCamera::GetInstance()->GetRotate().x >= 0.2f) {
			MainCamera::GetInstance()->mWorldTransform->rotation.x -= 0.01f;
		}

		// カメラの回転が終了したら
		else {

			// 黒帯を縮小
			if (mMovieScreen[0]->GetSprite()->GetScale().y > 0.0f) {

				// スケールを小さくする
				for (int32_t i = 0; i < mMovieScreen.size(); i++) {
					Vector2 scale = mMovieScreen[i]->GetSprite()->GetScale();
					mMovieScreen[i]->GetSprite()->SetScale({ scale.x, scale.y - 0.05f });
				}
			}
			else if (mMovieScreen[0]->GetSprite()->GetScale().y <= 0.0f) {

				// スケール修正
				for (int32_t i = 0; i < mMovieScreen.size(); i++) {
					mMovieScreen[i]->GetSprite()->SetScale({ 1.0f,0.0f });
				}

				// フェーズ移行
				mPhase = Phase::BATTLE;
				// カメラ操作を有効にする
				MainCamera::GetInstance()->SetCameraRotateControll(true);
				// ポーズボタンを表示する
				mPauseButtonUI->SetActive(true);
			}
		}

	}

	// プレイヤーの座標を移動
	mPlayer->Fall();
	// プレイヤー 更新
	mPlayer->UpdateObject();
	// ボス 更新
	mBoss->UpdateObject();

}

void GameScene::BattlePhase() {

	// プレイヤーのHPが0になったら
	if (mPlayer->GetStatus()->HP <= 0.0f) {
		mPhase = Phase::LOSE;
		mViggnetTime = 0.0f;
		PostEffect::GetInstance()->SetRedViggnetEnable(true);
		// ポーズボタンを非表示にする
		mPauseButtonUI->SetActive(false);

	}
	// ボスのHPが0になったら
	if (mBoss->GetStatus()->HP <= 0.0f) {
		mPhase = Phase::WIN;
		mViggnetTime = 0.0f;
		mFinishUI.isActive = true;
		mBoss->GetObject3D()->mSkinning->SetNextAnimation("Actor_Death");
		Framerate::GetInstance()->SetBattleSpeed(0.5f);
		// ポーズボタンを非表示にする
		mPauseButtonUI->SetActive(false);
	}

	// ヒットストップの更新
	UpdateHitStop();

	// プレイヤーが走っている場合
	if (mPlayer->GetObject3D()->mSkinning->GetNowSkinCluster()->name == "Actor_Run") {
		// 発生座標の更新
		mDashSmoke->SetEmitterPos(mPlayer->GetObject3D()->GetWorldTransform()->GetWorldPosition());
		// ダッシュ煙を発生させる
		mDashSmoke->Update();
	}

	// ヒットストップ中は演出以外を停止
	if (mHitStopTimer > 0.0f) {

		// 早期リターン
		return;
	}

	// プレイヤー 更新
	mPlayer->Update();
	// ボス 更新
	mBoss->Update();

	// 簡易押し出し処理
	GameManager::GetInstance()->ResolveCollision();

	// プレイヤー オブジェクト更新
	mPlayer->UpdateObject();
	// ボス オブジェクト更新
	mBoss->UpdateObject();
	

	// ゲームマネージャ 更新
	GameManager::GetInstance()->Update();

	// 斬撃エフェクト
	mPlayerTrailEffect->Update();
	if (mPlayer->GetBehavior() == Behavior::kAttack) {

		mPlayerTrailEffect->Create(
			*mPlayer->GetWorldPositionSword(0), 
			*mPlayer->GetWorldPositionSword(1));

		if (mPlayerTrailEffect->GetPositionFlag()) {
		}
	}

	mBossTrailEffect->Update();
	if (mBoss->GetIsOperating() == true) {
		if (mBossTrailEffect->GetPositionFlag()) {
			mBossTrailEffect->Create(*mBoss->GetWorldPositionSword(0), *mBoss->GetWorldPositionSword(1));
		}
	}

}

void GameScene::LosePhase() {

	// カメラ操作強制OFF
	MainCamera::GetInstance()->SetCameraRotateControll(false);

	mPlayer->GetObject3D()->GetModel()->GetMaterialData()->color = { 1.0f,1.0f,1.0f,1.0f };

	if (mIsGameOverSelect == false) {
		if (mViggnetTime < 1.0f) {
			mViggnetTime += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();

			// ビネットでフェードする
			PostEffect::GetInstance()->SetViggnetMultiplier(ExponentialInterpolation(10.0f, 20.0f, mViggnetTime, 1.0f));
			PostEffect::GetInstance()->SetViggnetIndex(ExponentialInterpolation(0.0f, 4.0f, mViggnetTime, 1.0f));
			PostEffect::GetInstance()->SetRedViggnetMultiplier(ExponentialInterpolation(0.0f, 0.5f, mViggnetTime, 1.0f));
			PostEffect::GetInstance()->SetRedViggnetIndex(ExponentialInterpolation(0.0f, 0.5f, mViggnetTime, 1.0f));
			//mGameOverFadeSprite->SetColor(Color(0.4f, 0.0f, 0.0f, ExponentialInterpolation(0.0f, 0.8f, viggnetTime, 1.0f)));

		}
		else if (mViggnetTime > 1.0f) {
			mIsGameOverSelect = true;
		}
	}
	else
	{

		if (mMessageFadeTime < 1.0f) {
			mMessageFadeTime += 1.0f / 30.0f;
			mViggnetTime = 0.0f;
		}
		else if (mMessageFadeTime > 1.0f) {
			mMessageFadeTime = 1.0f;
		}
		else if (mMessageFadeTime == 1.0f) {

			if (mViggnetTime == 0.0f) {
				if (InputManager::GetInstance()->GetPused(Gamepad::Button::UP) ||
					InputManager::GetInstance()->GetPused(Gamepad::Button::DOWN)) {
					if (mGameOverSelectUICount == 0) {
						mGameOverSelectUICount = 1;
					}
					else {
						mGameOverSelectUICount = 0;
					}
				}

				if (InputManager::GetInstance()->GetPused(Gamepad::Button::B)) {
					mViggnetTime += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
				}
			}
			else if (mViggnetTime < 1.0f) {

				mViggnetTime += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();

				// ビネットでフェードする
				PostEffect::GetInstance()->SetViggnetMultiplier(ExponentialInterpolation(20.0f, 10.0f, mViggnetTime, 1.0f));
				PostEffect::GetInstance()->SetViggnetIndex(ExponentialInterpolation(4.0f, 10.0f, mViggnetTime, 1.0f));
				PostEffect::GetInstance()->SetRedViggnetMultiplier(ExponentialInterpolation(0.0f, 0.5f, mViggnetTime, 1.0f));
				PostEffect::GetInstance()->SetRedViggnetIndex(ExponentialInterpolation(0.0f, 0.5f, mViggnetTime, 1.0f));
			}
			else if (mViggnetTime >= 1.0f) {
				PostEffect::GetInstance()->SetRedViggnetEnable(false);
				PostEffect::GetInstance()->SetRedViggnetMultiplier(0.0f);
				PostEffect::GetInstance()->SetRedViggnetIndex(0.0f);

				if (mGameOverSelectUICount == 0) {
					this->Init();
				}
				else {
					SceneManager::GetInstance()->ChangeScene("Title");
				}
			}


			switch (mGameOverSelectUICount)
			{
			case 0:
				mGameOverSelectUI[0]->SetScale(Vector2(1.2f, 1.2f));
				mGameOverSelectUI[1]->SetScale(Vector2(1.0f, 1.0f));

				break;
			case 1:

				mGameOverSelectUI[0]->SetScale(Vector2(1.0f, 1.0f));
				mGameOverSelectUI[1]->SetScale(Vector2(1.2f, 1.2f));

				break;

			default:
				break;
			}

		}

		mGameOverMessageSprite->SetColor(Color(1.0f, 1.0f, 1.0f, ExponentialInterpolation(0.0f, 1.0f, mMessageFadeTime, 1.0f)));

		for (int32_t i = 0; i < mGameOverSelectUI.size(); i++) {
			mGameOverSelectUI[i]->SetColor(Color(1.0f, 1.0f, 1.0f, ExponentialInterpolation(0.0f, 1.0f, mMessageFadeTime, 1.0f)));
		}

	}


	// UI 更新処理
	mGameOverFadeSprite->Update();
	mGameOverMessageSprite->Update();

	for (int32_t i = 0; i < mGameOverSelectUI.size(); i++) {
		mGameOverSelectUI[i]->Update();
	}

}

void GameScene::WinPhase() {

	// カメラ操作強制OFF
	MainCamera::GetInstance()->SetCameraRotateControll(false);

	if (mFinishUI.isActive) {

		// 終了時のUI表示が終了していない場合
		if (!mIsFinishUIDisplayEnd) {

			// ボスの身体に炎を発生させる
			// 発生座標の更新
			mFlameParticle->SetEmitterPos(mBoss->GetBodyPos());
			mFlameParticle->Update();


			// UI 透明度操作
			if (mFinishUI.t < 1.0f) {
				mFinishUI.t += (10.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
			}
			// 終了時処理
			else if (mFinishUI.t >= 1.0f) {
				mFinishUI.t = 1.0f;
				if (mFinithUIDisplsyTime == 0.0f) {
					mFinithUIDisplsyTime = kFinithUIDisplsyTimeMax;
				}

				// 一定時間UIを表示
				if (mFinithUIDisplsyTime > 0.0f) {
					mFinithUIDisplsyTime -= (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
				}
				// 終了時処理
				else if (mFinithUIDisplsyTime <= 0.0f) {
					mFinithUIDisplsyTime = 0.0f;
					mIsFinishUIDisplayEnd = true;
					Framerate::GetInstance()->SetBattleSpeed(1.0f);
				}

			}


		}
		// 終了時のUI表示が終了している場合
		else {

		
			// UIを透明にする
			if (mFinishUI.t > 0.0f) {
				mFinishUI.t -= (2.0f / (Framerate::GetInstance()->GetFramerate())) * Framerate::GetInstance()->GetGameSpeed();
			}
			// 終了時の処理
			else if (mFinishUI.t <= 0.0f) {
				mFinishUI.t = 0.0f;
				mFinishUI.isActive = false;
			}

		}

		mFinishUI.sprite->SetColor(Color(1.0f, 1.0f, 1.0f, ExponentialInterpolation(0.0f, 1.0f, mFinishUI.t, 1.0f)));
	}

	if (mIsFinishUIDisplayEnd && mFinishUI.t <= 0.0f) {
		if (mBoss->GetObject3D()->mSkinning->GetIsNowAnimationFinished()) {
			if (mViggnetTime < 1.0f) {
				mViggnetTime += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
			}
			else if (mViggnetTime >= 1.0f) {
				mViggnetTime = 1.0f;
			}
		}
	}
	// ビネットでフェードインする
	PostEffect::GetInstance()->SetViggnetIndex(ExponentialInterpolation(0.0f, 10.0f, mViggnetTime, 1.0f));

	// フェードインが終わったら
	if (mViggnetTime == 1.0f) {
		// リザルトシーンへ移行する
		SceneManager::GetInstance()->ChangeScene("Result");
	}

	// プレイヤー 更新
	mPlayer->UpdateObject();
	// ボス 更新
	mBoss->UpdateObject();
	// UI 更新処理
	mFinishUI.sprite->Update();

}


void GameScene::Draw() {

	// Skyboxの描画前処理
	Skybox::GetInstance()->PreDraw();
	Skybox::GetInstance()->Draw();

	// レベルデータ読み込み
	LevelEditor::GetInstance()->Draw();

	// Object3D(3DModel)の描画前処理
	ModelManager::GetInstance()->PreDraw();

	//mGroundShadow[0]->Draw();

	mPlayer->ColliderDraw();
	mBoss->ColliderDraw();

	mPlayerTrailEffect->Draw();
	mBossTrailEffect->Draw();

	// Object3D(Skinning)の描画前処理
	ModelManager::GetInstance()->PreDrawForSkinning();

	mPlayer->Draw();
	mBoss->Draw();

	// パーティクル
	ParticleManager::GetInstance()->PreDraw();
	ParticleManager::GetInstance()->Draw();

}

void GameScene::DrawUI()
{

	// 2DSprite(画像)の描画前処理
	SpriteAdministrator::GetInstance()->PreDraw();

	switch (mPhase)
	{
	case Phase::BEGIN:
		break;
	case Phase::BATTLE:

		Vector2 hpPos = MainCamera::GetInstance()->GetWindowSize();
		mPlayer->GetStatus()->Draw(Vector2(hpPos.x * 0.5f, hpPos.y * 0.8f));
		mBoss->GetStatus()->Draw(Vector2(hpPos.x * 0.5f, hpPos.y * 0.2f));

		
		// ボタン入力 & 行動表示
		for (int32_t i = 0; i < mButtonUI.size(); i++) {
			mButtonUI[i]->Draw();
			if (i != 2) {
				mActionsUI[i]->Draw();
			}
		}

		// ボタン入力時
		if (InputManager::GetInstance()->GetLongPush(Gamepad::Button::A) ||
			InputManager::GetInstance()->GetPused(Gamepad::Button::A)) {
			// 緑色に変更
			mButtonUI[0]->SetColor(Color(0.0f, 0.0f, 0.8f, 1.0f));
		}
		else {
			// 非入力時は白色
			mButtonUI[0]->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		}

		if (InputManager::GetInstance()->GetLongPush(Gamepad::Button::B) ||
			InputManager::GetInstance()->GetPused(Gamepad::Button::B)) {
			// 緑色に変更
			mButtonUI[1]->SetColor(Color(0.0f, 0.0f, 0.8f, 1.0f));
		}
		else {
			// 非入力時は白色
			mButtonUI[1]->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		}

		if (InputManager::GetInstance()->GetLongPush(Gamepad::Button::Y) ||
			InputManager::GetInstance()->GetPused(Gamepad::Button::Y)) {
			// 緑色に変更
			mButtonUI[2]->SetColor(Color(0.0f, 0.0f, 0.8f, 1.0f));
		}
		else {
			// 非入力時は白色
			mButtonUI[2]->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		}

		if (InputManager::GetInstance()->GetLongPush(Gamepad::Button::X) ||
			InputManager::GetInstance()->GetPused(Gamepad::Button::X)) {
			// 緑色に変更
			mButtonUI[3]->SetColor(Color(0.0f, 0.0f, 0.8f, 1.0f));
		}
		else {
			// 非入力時は白色
			mButtonUI[3]->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		}


		// レティクル表示
		mPlayer->GetReticle3D()->Draw2DReticle();

		// ダメージ画像の表記
		DamageReaction::GetInstance()->DrawSprite();

		break;
	case Phase::LOSE:

		mGameOverFadeSprite->Draw();
		mGameOverMessageSprite->Draw();


		for (int32_t i = 0; i < mGameOverSelectUI.size(); i++) {
			mGameOverSelectUI[i]->Draw();
		}

		break;
	case Phase::WIN:

		mFinishUI.sprite->Draw();

		break;
	default:
		break;
	}


}

void GameScene::StartHitStop(float duration)
{
	mIsHitStopActive = true;
	mHitStopDuration = duration;
	mHitStopTimer = 0.0f;
}

void GameScene::UpdateHitStop() {

	// ゲームマネージャからヒットストップのリクエストを受け付ける
	if (!mIsHitStopActive) {
		GameManager* gameManager = GameManager::GetInstance();
		if (gameManager->GetHitStopRequest()) {

			// ヒットストップの発生時間を取得する
			mHitStopDuration = gameManager->GetHitStopDuration();
			// ヒットストップの実行
			mIsHitStopActive = true;

		}
	}

	// HitStop中の処理
	if (mIsHitStopActive) {

		// タイマーを更新
		mHitStopTimer += 1.0f / Framerate::GetInstance()->GetFramerate();
		if (mHitStopTimer >= mHitStopDuration) {
			mIsHitStopActive = false;
			mHitStopTimer = 0.0f;
			mHitStopDuration = 0.0f;
		}
	}
}

void GameScene::InitActor(){

	// プレイヤー
	mPlayer = std::make_unique<Player>();
	mPlayer->Init();
	mPlayer->GetObject3D()->SetTranslate(Vector3(0.0f, 100.0f, -8.0f));

	// ボス
	mBoss = std::make_unique<BossEnemy>();
	mBoss->Init();

	// 互いのポインタを渡す
	mPlayer->SetBoss(mBoss.get());
	mBoss->SetTarget(mPlayer.get());

	// オブジェクトにCubeMapのテクスチャ番号を渡す
	mPlayer->GetModel()->SetCubeTexture(Skybox::GetInstance()->mTextureHandle);
	mPlayer->GetCollider()->GetModel()->SetCubeTexture(Skybox::GetInstance()->mTextureHandle);
	mBoss->GetModel()->SetCubeTexture(Skybox::GetInstance()->mTextureHandle);
	mBoss->GetCollider()->GetModel()->SetCubeTexture(Skybox::GetInstance()->mTextureHandle);
	mPlayer->GetReticle3D()->SetCubeMap(Skybox::GetInstance()->mTextureHandle);

}

void GameScene::InitUI(){

	MainCamera* mainCamera = MainCamera::GetInstance();

	// Pause 
	UIManager::GetInstance()->CreateUI("PauseButtonUI", SceneName::Game);
	mPauseButtonUI = static_cast<BaseUI*>(UIManager::GetInstance()->GetUIPtr("PauseButtonUI"));
	// 最初は非表示
	mPauseButtonUI->SetActive(false);

	// 演出中、スクリーンの上下に表示する黒画像

	// 上部の黒画像
	UIManager::GetInstance()->CreateUI("MovieScreenTop", SceneName::Game);
	mMovieScreen[0] = static_cast<BaseUI*>(UIManager::GetInstance()->GetUIPtr("MovieScreenTop"));
	mMovieScreen[0]->GetSprite()->SetSpriteSize({mainCamera->GetWindowSize().x,mainCamera->GetWindowSize().y * 0.1f});
	mMovieScreen[0]->SetPos(Vector2(mainCamera->GetWindowSize().x * 0.5f, 0.0f));


	// 下部の黒画像
	UIManager::GetInstance()->CreateUI("MovieScreenBottom", SceneName::Game);
	mMovieScreen[1] = static_cast<BaseUI*>(UIManager::GetInstance()->GetUIPtr("MovieScreenBottom"));
	mMovieScreen[1]->GetSprite()->SetSpriteSize({ mainCamera->GetWindowSize().x,mainCamera->GetWindowSize().y * 0.1f });
	mMovieScreen[1]->SetPos(Vector2(mainCamera->GetWindowSize().x * 0.5f, mainCamera->GetWindowSize().y));

	// ボタン入力
	for (int32_t i = 0; i < mButtonUI.size(); i++) {
		mButtonUI[i] = std::make_shared<Sprite>();
		mButtonUI[i]->Init();
		std::string directryPath = "UI/Button/";
		std::string filepath;

		// 基準になる座標(右下に配置)
		Vector2 basePos{};
		basePos.x = mainCamera->GetWindowSize().x * 0.9f;
		basePos.y = mainCamera->GetWindowSize().y * 0.9f;

		switch (i) {
		case 0:
			filepath = "A";

			// 下位置に配置 yのみ下にずらす
			mButtonUI[i]->SetPos({ basePos.x, basePos.y + kButtonSpacing.y });

			break;
		case 1:
			filepath = "B";
			// 右位置に配置 xのみ右にずらす
			mButtonUI[i]->SetPos({ basePos.x + kButtonSpacing.x, basePos.y });
			break;
		case 2:
			filepath = "Y";
			// 上位置に配置 yのみ上にずらす
			mButtonUI[i]->SetPos({ basePos.x, basePos.y - kButtonSpacing.y });
			break;
		case 3:
			filepath = "X";
			// 左位置に配置 xのみ左にずらす
			mButtonUI[i]->SetPos({ basePos.x - kButtonSpacing.x, basePos.y });
			break;
		case 4:
			filepath = "RB";
			// 左位置に配置 xを左、yを上ににずらす
			mButtonUI[i]->SetPos({ basePos.x - kButtonSpacing.x, basePos.y - (kButtonSpacing.y * 2.5f) });
			break;
		default:
			break;
		}
		filepath += ".png";
		mButtonUI[i]->SetTexture(directryPath + filepath);
		mButtonUI[i]->SetScale({ 0.15f,0.15f });
		mButtonUI[i]->SetAnchorPoint(Vector2(0.5f, 0.5f));
	}

	// 行動内容表示UI
	for (int32_t i = 0; i < mActionsUI.size(); i++) {
		// スプライト生成
		mActionsUI[i] = std::make_shared<Sprite>();
		mActionsUI[i]->Init();

		if (i < 2 || i == 4) {
			// ボタンの右側に配置
			mActionsUI[i]->SetPos(Vector2(mButtonUI[i]->GetPos().x + kActionSpacing, mButtonUI[i]->GetPos().y));
			mActionsUI[i]->SetAnchorPoint(Vector2(0.0f, 0.5f));
		}
		else {
			// ボタンの左側に配置
			mActionsUI[i]->SetPos(Vector2(mButtonUI[i]->GetPos().x - kActionSpacing, mButtonUI[i]->GetPos().y));
			mActionsUI[i]->SetAnchorPoint(Vector2(1.0f, 0.5f));
		}

		mActionsUI[i]->SetScale({ 0.2f,0.2f });
	}
	// テクスチャをセット
	mActionsUI[0]->SetTexture("UI/Fonts/Avoid.png");
	mActionsUI[1]->SetTexture("UI/Fonts/ATK.png");
	mActionsUI[2]->SetTexture("UI/Fonts/SATK.png");
	mActionsUI[3]->SetTexture("UI/Fonts/RockOn.png");
	mActionsUI[4]->SetTexture("UI/Fonts/Dash.png");

	// 動くオブジェクトの地面影
	for (int32_t i = 0; i < mGroundShadow.size(); i++) {
		mGroundShadow[i] = std::make_unique<Object3d>();
		mGroundShadow[i]->Init("groundshadow" + std::to_string(i));
		mGroundShadow[i]->SetModel("groundShadow.gltf");
	}

	mGameOverFadeSprite = std::make_unique<Sprite>();
	mGameOverFadeSprite->Init();
	mGameOverFadeSprite->SetTexture("white2x2.png");
	mGameOverFadeSprite->SetSpriteSize(Vector2(1280.0f, 720.0f));
	mGameOverFadeSprite->SetColor(Color(0.8f, 0.0f, 0.0f, 0.0f));

	mGameOverMessageSprite = std::make_unique<Sprite>();
	mGameOverMessageSprite->Init();
	mGameOverMessageSprite->SetTexture("UI/System/Lose.png");
	mGameOverMessageSprite->SetAnchorPoint(Vector2(0.5f, 0.5f));
	mGameOverMessageSprite->SetPos(Vector2(640.0f, 100.0f));
	mGameOverMessageSprite->SetSpriteSize(Vector2(768.0f, 256.0f));
	mGameOverMessageSprite->SetColor(Color(1.0f, 1.0f, 1.0f, 0.0f));
	mMessageFadeTime = 0.0f;

	for (int32_t i = 0; i < mGameOverSelectUI.size(); i++) {
		mGameOverSelectUI[i] = std::make_unique<Sprite>();
		mGameOverSelectUI[i]->Init();
		if (i == 0) {
			mGameOverSelectUI[0]->SetTexture("UI/System/Retry.png");
		}
		else {
			mGameOverSelectUI[1]->SetTexture("UI/System/BackW.png");
		}
		mGameOverSelectUI[i]->SetAnchorPoint(Vector2(0.5f, 0.5f));
		mGameOverSelectUI[i]->SetPos(Vector2(640.0f, ((i + 2) * 100.0f + 300.0f)));
		mGameOverSelectUI[i]->SetSpriteSize(Vector2(256.0f, 64.0f));
		mGameOverSelectUI[i]->SetColor(Color(1.0f, 1.0f, 1.0f, 0.0f));
	}
	mGameOverSelectUICount = 0;
	mIsGameOverSelect = false;


	// ゲームクリア演出
	mFinishUI.sprite = std::make_unique<Sprite>();
	mFinishUI.sprite->Init();
	mFinishUI.sprite->SetTexture("Finish.png");
	mFinishUI.sprite->SetAnchorPoint(Vector2(0.5f, 0.5f));
	mFinishUI.sprite->SetPos(Vector2(640.0f, 200.0f));
	mFinishUI.sprite->SetSpriteSize(Vector2(768.0f, 256.0f));
	mFinishUI.sprite->SetColor(Color(1.0f, 1.0f, 1.0f, 0.0f));
	mFinithUIDisplsyTime = 0.0f;
	mIsFinishUIDisplayEnd = false;

}

void GameScene::InitCamera(){

	// メインカメラをフォローカメラ仕様にする
	MainCamera* mainCamera = MainCamera::GetInstance();
	mainCamera->SetFollowTarget(mPlayer->GetObject3D()->GetWorldTransform());
	mainCamera->SetSearchTarget(mBoss->GetObject3D()->GetWorldTransform());
	mainCamera->SetTranslate(Vector3(0.0f, 60.0f, -5.0f));
	mainCamera->SetRotate(Vector3(1.57f, 0.0f, 0.0f));

	// スタート演出
	mPlayerStartAndEnd.start = { 0.0f,0.0f,-36.0f };
	mPlayerStartAndEnd.end = { 0.0f,0.0f,-8.0f };
	mPlayerStartAndEnd.t = 0.0f;
	mPlayerStartAndEnd.k = 1.0f;

	mCameraRot.start = { 1.57f,6.28f,0.0f };
	mCameraRot.end = { 0.0f,0.0f,0.0f };
	mCameraRot.t = 0.0f;
	mCameraRot.k = 0.0f;

	mCameraTr.start = { 0.0f,60.0f,0.0f };
	mCameraTr.end = { 0.0f,1.7f,-15.5f };
	mCameraTr.t = 0.0f;
	mCameraTr.k = 0.0f;

}

void GameScene::InitEffects(){

	// ダッシュ煙 初期化
	mDashSmoke = std::make_unique<ParticleEmitter>("DashSmoke");
	mDashSmoke->Init();
	ParticleManager::GetInstance()->CreateParticleGroupe(mDashSmoke->mName, "circle.png");

	// 炎パーティクル
	mFlameParticle = std::make_unique<ParticleEmitter>("Flame");
	mFlameParticle->Init();
	ParticleManager::GetInstance()->CreateParticleGroupe(mFlameParticle->mName, "Flame.png");

	// ヒットパーティクル
	mHitParticle = std::make_unique<ParticleEmitter>("Hit");
	mHitParticle->Init();
	ParticleManager::GetInstance()->CreateParticleGroupe(mHitParticle->mName, "Flame.png");

	// 軌道エフェクト
	mPlayerTrailEffect = std::make_unique<TrailEffect>();
	mPlayerTrailEffect->Init();
	mPlayerTrailEffect->SetColor(Color(0.1f, 0.1f, 1.0f, 0.8f));

	mBossTrailEffect = std::make_unique<TrailEffect>();
	mBossTrailEffect->Init();
	mBossTrailEffect->SetColor(Color(1.0f, 0.1f, 0.1f, 0.8f));

	// 開始前のビネット
	mViggnetTime = 0.0f;


}

void GameScene::InitObject(){

	// モデル 読み込み
	ModelManager::GetInstance()->LoadModel("plane", "plane.gltf");
	ModelManager::GetInstance()->LoadModel("field", "field.gltf");
	ModelManager::GetInstance()->LoadModel("box", "box.gltf");
	ModelManager::GetInstance()->LoadModel("ball", "ball.obj");
	ModelManager::GetInstance()->LoadModel("AnimatedCube", "AnimatedCube.gltf");
	ModelManager::GetInstance()->LoadModel("simpleSkin", "simpleSkin.gltf");

	ModelManager::GetInstance()->LoadModel("boss", "boss.gltf");
	ModelManager::GetInstance()->LoadModel("Weapons", "sword.gltf");
	ModelManager::GetInstance()->LoadModel("Shield", "Shield.gltf");
	ModelManager::GetInstance()->LoadModel("groundShadow", "groundShadow.gltf");
	ModelManager::GetInstance()->LoadModel("EarthSwordBullet", "EarthSwordBullet.gltf");

	// SkyBox 読み込み
	TextureManager::GetInstance()->LoadTexture("skybox/rostock_laage_airport_4k.dds");
	// skybox
	Skybox::GetInstance()->Init("skybox", "skybox.dds");
	// LevelEditorでSkyBoxのテクスチャを参照
	LevelEditor::GetInstance()->SetTextureCubeMap(Skybox::GetInstance()->mTextureHandle);


}
