#include "GameScene.h"
#include "GameEngine/Object/Sprite/SpriteAdministrator.h"
#include <cassert>
#include <fstream>
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Editor/LevelEditor.h"
#include "App/Reaction/DamageReaction.h"
#include "GameEngine/Effect/Particle/ParticleManager.h"
#include "App/Status/StatusManager.h"
#include "GameEngine/Effect/PostEffect/PostEffect.h"
#include "GameEngine/GameMaster/Framerate.h"
#include "GameEngine/Scene/SceneManager.h"

void GameScene::Final() {}

void GameScene::Init() {

	// 衝突判定マネージャ
	mCollisionManager = std::make_unique<CollisionManager>();

	// ステータスマネージャ
	StatusManager::GetInstance()->Init();

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

	// SkyBox 読み込み
	TextureManager::GetInstance()->LoadTexture("skybox/rostock_laage_airport_4k.dds");

	// skyboxの宣言
	mSkybox = Skybox::GetInstance();
	mSkybox->Init("skybox", "rostock_laage_airport_4k.dds");

	// プレイヤー
	mPlayer = std::make_unique<Player>();
	mPlayer->Init();

	// ボス
	mBoss = std::make_unique<BossEnemy>();
	mBoss->Init();

	// 互いのポインタを渡す
	mPlayer->SetBoss(mBoss.get());
	mBoss->SetTarget(mPlayer.get());

	// オブジェクトにCubeMapのテクスチャ番号を渡す
	mPlayer->GetModel()->SetCubeTexture(mSkybox->mTextureHandle);
	mPlayer->GetCollider()->GetModel()->SetCubeTexture(mSkybox->mTextureHandle);
	mBoss->GetModel()->SetCubeTexture(mSkybox->mTextureHandle);
	mBoss->GetCollider()->GetModel()->SetCubeTexture(mSkybox->mTextureHandle);
	mPlayer->GetReticle3D()->SetCubeMap(mSkybox->mTextureHandle);
	LevelEditor::GetInstance()->SetTextureCubeMap(mSkybox->mTextureHandle);


	// パーティクル
	//ParticleManager::GetInstance()->Init();
	//mDTCParticle = std::make_unique<DiffusionToCircleParticle>();
	//mDTCParticle->Init();


	// 軌道エフェクト
	mPlayerTrailEffect = std::make_unique<TrailEffect>();
	mPlayerTrailEffect->Init();
	mPlayerTrailEffect->SetColor(Color(0.1f, 0.1f, 1.0f, 0.8f));

	mBossTrailEffect = std::make_unique<TrailEffect>();
	mBossTrailEffect->Init();
	mBossTrailEffect->SetColor(Color(1.0f, 0.1f, 0.1f, 0.8f));

	// ゲームシーンの段階
	mPhase = Phase::BEGIN;
	// 開始前のビネット
	mViggnetTime = 0.0f;

	// 以下 UI
	mActionUI.sprite = std::make_unique<Sprite>();
	mActionUI.sprite->Init();
	mActionUI.sprite->SetTexture("UI/UI0.png");
	mActionUI.sprite->SetScale({ 0.5f,0.5f });
	mActionUI.sprite->SetPos({ 640.0f,600.0f });
	mActionUI.sprite->SetAnchorPoint({ 0.5f,0.5f });
	mActionUI.t = 0.0f;
	mActionUI.displayCount = 0.0f;
	mActionUI.isActive = true;

	// 以下 UI
	mMoveUI.sprite = std::make_unique<Sprite>();
	mMoveUI.sprite->Init();
	mMoveUI.sprite->SetTexture("UI/UI1.png");
	mMoveUI.sprite->SetScale({ 0.5f,0.5f });
	mMoveUI.sprite->SetPos({ 640.0f,500.0f });
	mMoveUI.sprite->SetAnchorPoint({ 0.5f,0.5f });
	mMoveUI.t = 0.0f;
	mMoveUI.displayCount = 0.0f;
	mMoveUI.isActive = true;

	// ボタン入力
	for (int32_t i = 0; i < mButtonUI.size(); i++) {
		mButtonUI[i] = std::make_shared<Sprite>();
		mButtonUI[i]->Init();
		std::string directryPath = "UI/Button/";
		std::string filepath;

		// 基準になる座標(右下に配置)
		Vector2 basePos{};
		basePos.x = MainCamera::GetInstance()->GetWindowSize().x * 0.9f;
		basePos.y = MainCamera::GetInstance()->GetWindowSize().y * 0.9f;

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

		if (i < 2) {
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

	// 動くオブジェクトの地面影
	for (int32_t i = 0; i < mGroundShadow.size(); i++) {
		mGroundShadow[i] = std::make_unique<Object3d>();
		mGroundShadow[i]->Init("groundshadow" + std::to_string(i));
		mGroundShadow[i]->SetModel("groundShadow.gltf");
	}

	// スタート演出
	mPlayerStartAndEnd.start = { 0.0f,0.0f,-36.0f };
	mPlayerStartAndEnd.end = { 0.0f,0.0f,-8.0f };
	mPlayerStartAndEnd.t = 0.0f;
	mPlayerStartAndEnd.k = 1.0f;

	MainCamera::GetInstance()->SetTranslate(Vector3(0.0f, 60.0f, 0.0f));
	MainCamera::GetInstance()->SetRotate(Vector3(1.57f, 12.56f, 0.0f));

	mCameraRot.start = { 1.57f,6.28f,0.0f };
	mCameraRot.end = { 0.0f,0.0f,0.0f };
	mCameraRot.t = 0.0f;
	mCameraRot.k = 0.0f;

	mCameraTr.start = { 0.0f,60.0f,0.0f };
	mCameraTr.end = { 0.0f,1.7f,-15.5f };
	mCameraTr.t = 0.0f;
	mCameraTr.k = 0.0f;

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

void GameScene::Update() {

	mMoveUI.sprite->Update();
	mActionUI.sprite->Update();

	// ボタン入力 & 行動表示
	for (int32_t i = 0; i < mButtonUI.size(); i++) {
		mButtonUI[i]->Update();
		mActionsUI[i]->Update();
	}


	// シーン切り替え
	// 1キーを押したうえで
	// Bボタンまたは、Enterキーでシーン遷移
	if (InputManager::GetInstance()->GetPushKey(DIK_1)) {
		if (InputManager::GetInstance()->GetPused(Gamepad::Button::B) || InputManager::GetInstance()->GetTriggerKey(DIK_RETURN)) {
			// リザルトシーンへ移行する
			SceneManager::GetInstance()->ChangeScene("Result");
		}
	}

	if (InputManager::GetInstance()->GetPushKey(DIK_R)) {
		Init();
	}

	switch (mPhase)
	{
	case Phase::BEGIN:

		// ビネットでフェードインする
		if (mViggnetTime < 1.0f) {

			// プレイヤーの初期位置を設定
			if (mViggnetTime == 0.0f) {

				// プレイヤーの初期位置を設定
				mPlayer->GetObject3D()->mWorldTransform->translation = ExponentialInterpolation(
					mPlayerStartAndEnd.start, mPlayerStartAndEnd.end, mPlayerStartAndEnd.t, mPlayerStartAndEnd.k);
			}

			// ビネットの時間を進める
			mViggnetTime += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
		}
		// ビネットの時間が1.0fを超えたら1.0fに設定
		else if (mViggnetTime >= 1.0f) {
			mViggnetTime = 1.0f;
		}

		// ビネットでフェードインする
		PostEffect::GetInstance()->SetViggnetIndex(ExponentialInterpolation(10.0f, 0.0f, mViggnetTime, 1.0f));

		// フェードインが終わったら戦闘開始
		if (mViggnetTime == 1.0f) {

			if (mCameraTr.k < 1.0f || mCameraRot.k < 1.0f) {

				mCameraRot.t += (1.0f / (Framerate::GetInstance()->GetFramerate() * 6.0f)) * Framerate::GetInstance()->GetGameSpeed();
				if (mCameraRot.t > 1.0f) {
					mCameraRot.t = 1.0f;
				}

				if (mCameraRot.t > 0.5f) {
					mCameraTr.t += (1.0f / (Framerate::GetInstance()->GetFramerate() * 2.0f)) * Framerate::GetInstance()->GetGameSpeed();
					if (mCameraTr.t > 1.0f) {
						mCameraTr.t = 1.0f;
					}
				}

				if (mCameraRot.t >= 1.0f) {
					mCameraRot.k += (2.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
					if (mCameraRot.k > 1.0f) {
						mCameraRot.k = 1.0f;
					}
					MainCamera::GetInstance()->mWorldTransform->rotation.x = ExponentialInterpolation(mCameraRot.start.x, mCameraRot.end.x, mCameraRot.k, 1.0f);

				}
				if (mCameraRot.k >= 0.5f) {
					mCameraTr.k += (2.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();
					if (mCameraTr.k > 1.0f) {
						mCameraTr.k = 1.0f;
					}

					MainCamera::GetInstance()->mWorldTransform->translation.z = ExponentialInterpolation(mCameraTr.start.z, mCameraTr.end.z, mCameraTr.k, 1.0f);

				}

				MainCamera::GetInstance()->mWorldTransform->translation.y = ExponentialInterpolation(mCameraTr.start.y, mCameraTr.end.y, mCameraTr.t, 1.0f);
				MainCamera::GetInstance()->mWorldTransform->rotation.y = ExponentialInterpolation(mCameraRot.start.y, mCameraRot.end.y, mCameraRot.t, 1.0f);


			}
			else if (mPlayerStartAndEnd.t >= 1.0f && mCameraTr.k >= 1.0f && mCameraRot.k >= 1.0f) {

				// メインカメラをフォローカメラ仕様にする
				MainCamera::GetInstance()->SetFollowTarget(mPlayer->GetObject3D()->GetWorldTransform());
				MainCamera::GetInstance()->SetSearchTarget(mBoss->GetObject3D()->GetWorldTransform());

				// フェーズ移行
				mPhase = Phase::BATTLE;
			}
		}

		if (mPlayerStartAndEnd.t < 1.0f) {

			mPlayerStartAndEnd.t += (1.0f / (Framerate::GetInstance()->GetFramerate() * 4.0f)) * Framerate::GetInstance()->GetGameSpeed();

			if (mPlayerStartAndEnd.t >= 1.0f) {
				mPlayerStartAndEnd.t = 1.0f;
			}

			mPlayer->GetObject3D()->mWorldTransform->translation = ExponentialInterpolation(mPlayerStartAndEnd.start, mPlayerStartAndEnd.end, mPlayerStartAndEnd.t, mPlayerStartAndEnd.k);
		}

		// プレイヤー 更新
		mPlayer->UpdateObject();
		// ボス 更新
		mBoss->UpdateObject();

		break;
	case Phase::BATTLE:

		// ヒットストップの更新
		UpdateHitStop();

		// プレイヤーのHPが0になったら
		if (mPlayer->GetStatus()->HP <= 0.0f) {
			mPhase = Phase::LOSE;
			mViggnetTime = 0.0f;
			PostEffect::GetInstance()->SetRedViggnetEnable(true);

		}
		// ボスのHPが0になったら
		if (mBoss->GetStatus()->HP <= 0.0f) {
			mPhase = Phase::WIN;
			mViggnetTime = 0.0f;
			mFinishUI.isActive = true;
			mBoss->GetObject3D()->mSkinning->SetNextAnimation("death");
			Framerate::GetInstance()->SetBattleSpeed(0.5f);
		}

		// プレイヤー 更新
		mPlayer->Update();

		// ボス
		mBoss->Update();

		// コライダーリストへの追加処理
		mPlayer->SetCollider(mCollisionManager.get());
		mPlayer->SetColliderList(mCollisionManager.get());
		mBoss->SetCollider(mCollisionManager.get());
		mBoss->SetAttackCollider(mCollisionManager.get());

		// 衝突判定を行う
		mCollisionManager->Update();

		// コライダーリストのクリア
		mCollisionManager->ClearColliders();

		// 斬撃エフェクト
		mPlayerTrailEffect->Update();
		if (mPlayer->GetBehavior() == Behavior::kAttack) {
			if (mPlayerTrailEffect->GetGetPositionFlag()) {
				mPlayerTrailEffect->Create(*mPlayer->GetWorldPositionSword(0), *mPlayer->GetWorldPositionSword(1));
			}
		}
		mBossTrailEffect->Update();
		if (mBossTrailEffect->GetGetPositionFlag()) {
			mBossTrailEffect->Create(*mBoss->GetWorldPositionSword(0), *mBoss->GetWorldPositionSword(1));
		}



		break;
	case Phase::LOSE:
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

		break;
	case Phase::WIN:

		if (mFinishUI.isActive) {

			// 終了時のUI表示が終了していない場合
			if (!mIsFinishUIDisplayEnd) {

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

		// フェードインが終わったら戦闘開始
		if (mViggnetTime == 1.0f) {
			//this->Init();
			// リザルトシーンへ移行する
			SceneManager::GetInstance()->ChangeScene("Result");
		}

		// プレイヤー 更新
		mPlayer->UpdateObject();
		// ボス 更新
		mBoss->UpdateObject();

		break;
	default:
		break;
	}


#ifdef _DEBUG
	mSkybox->DrawGUI("Skybox");
	mPlayer->DrawGUI();
	mBoss->DrawGUI();
	//mPlayerTrailEffect->DrawGui();
#endif // _DEBUG

	// カメラ
	MainCamera::GetInstance()->Update();

	// スカイボックス
	mSkybox->Update();

	// ステータスマネージャ
	StatusManager::GetInstance()->Update();

	// 地面影
	mGroundShadow[0]->mWorldTransform->translation = mPlayer->GetWorldPos();
	mGroundShadow[0]->Update();

	//// プレイヤーの現在HPに応じて赤色ビネットを変化させる
	//float playerHPRatio = (float(mPlayer->GetStatus()->MAXHP) - float(mPlayer->GetStatus()->HP)) / float(mPlayer->GetStatus()->MAXHP);
	//if (playerHPRatio > 0.3f) {

	//	if (playerHPRatio > 0.9f) {
	//		RenderCopyImage::GetInstance()->SetRedViggnetMultiplier(ExponentialInterpolation(2.8f, 0.8f, playerHPRatio, 1.0f));
	//	}

	//	RenderCopyImage::GetInstance()->SetRedViggnetEnable(true);
	//	RenderCopyImage::GetInstance()->SetRedViggnetIndex(ExponentialInterpolation(0.0f, 0.5f, playerHPRatio, 1.0f));
	//}

	// レベルエディタ更新/
	LevelEditor::GetInstance()->Update();

	// パーティクル
	//ParticleManager::GetInstance()->Update();
	//mDTCParticle->Update();

	mGameOverFadeSprite->Update();
	mGameOverMessageSprite->Update();

	for (int32_t i = 0; i < mGameOverSelectUI.size(); i++) {
		mGameOverSelectUI[i]->Update();
	}
	mFinishUI.sprite->Update();

}

void GameScene::Draw() {

	// Skyboxの描画前処理
	mSkybox->PreDraw();
	//skybox_->Draw();

	// レベルデータ読み込み
	LevelEditor::GetInstance()->Draw();

	// Object3D(3DModel)の描画前処理
	ModelManager::GetInstance()->PreDraw();

	//mGroundShadow[0]->Draw();

	mPlayer->ColliderDraw();
	mBoss->ColliderDraw();

	//mPlayerTrailEffect->Draw();
	//mBossTrailEffect->Draw();

	// Object3D(Skinning)の描画前処理
	ModelManager::GetInstance()->PreDrawForSkinning();

	mPlayer->Draw();
	mBoss->Draw();

	// パーティクル
	//ParticleManager::GetInstance()->PreDraw();
	//ParticleManager::GetInstance()->Draw();

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

		// UI表示
		//mMoveUI.sprite->Draw();
		//mActionUI.sprite->Draw();

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
		//DamageReaction::GetInstance()->DrawSprite();

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

	// プレイヤーまたは敵がHitStopをリクエストしているとき
	if (mPlayer->GetHitStopDuration() != 0.0f) {
		StartHitStop(mPlayer->GetHitStopDuration());
		Framerate::GetInstance()->SetBattleSpeed(0.25f);
		mPlayer->SetHitStopDuration(0.0f);
	}

	if (mBoss->GetHitStopDuration() != 0.0f) {
		StartHitStop(mBoss->GetHitStopDuration());
		Framerate::GetInstance()->SetBattleSpeed(0.5f);
		mBoss->SetHitStopDuration(0.0f);
	}

	// HitStop中の処理
	if (mIsHitStopActive) {

		// タイマーを更新
		mHitStopTimer += 1.0f / Framerate::GetInstance()->GetFramerate();
		if (mHitStopTimer >= mHitStopDuration) {
			mIsHitStopActive = false;
			Framerate::GetInstance()->SetBattleSpeed(1.0f);
		}
	}
}
