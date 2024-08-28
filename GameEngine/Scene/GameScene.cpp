#include "GameScene.h"
#include "GameEngine/Object/Sprite/SpriteAdministrator.h"
#include <cassert>
#include <fstream>
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Editor/LevelEditor.h"
#include "GameEngine/GameBase/Reaction/DamageReaction.h"
#include "GameEngine/Effect/Particle/ParticleManager.h"
#include "GameEngine/GameBase/Status/StatusManager.h"

void GameScene::Finalize(){}

void GameScene::Init(){
	// 入力を取得する
	mInput = InputManager::GetInstance();

	// 衝突判定マネージャ
	mCollisionManager = std::make_unique<CollisionManager>();

	// ステータスマネージャ
	StatusManager::GetInstance()->Init();

	// モデル 読み込み
	ModelManager::GetInstance()->LoadModel("plane", "plane.gltf");
	ModelManager::GetInstance()->LoadModel("box", "box.gltf");
	ModelManager::GetInstance()->LoadModel("ball", "ball.obj");
	ModelManager::GetInstance()->LoadModel("AnimatedCube", "AnimatedCube.gltf");
	ModelManager::GetInstance()->LoadModel("simpleSkin", "simpleSkin.gltf");
	ModelManager::GetInstance()->LoadModel("human", "sneakWalk.gltf");
	ModelManager::GetInstance()->LoadModel("human", "walk.gltf");
	ModelManager::GetInstance()->LoadModel("Bow", "BowBody.gltf");
	ModelManager::GetInstance()->LoadModel("Bow", "Bow.obj");
	ModelManager::GetInstance()->LoadModel("Arrow", "Arrow.gltf");
	ModelManager::GetInstance()->LoadModel("boss", "boss.gltf");
	ModelManager::GetInstance()->LoadModel("Weapons", "sword.gltf");
	ModelManager::GetInstance()->LoadModel("Shield", "Shield.gltf");

	// SkyBox 読み込み
	DirectXCommon::GetInstance()->srv_->LoadTexture("skybox/rostock_laage_airport_4k.dds");

	// skyboxの宣言
	skybox_ = Skybox::GetInstance();
	skybox_->Init("skybox", "rostock_laage_airport_4k.dds");

	// プレイヤー
	mPlayer = std::make_unique<Player>();
	mPlayer->Init();
	// ボス
	mBoss = std::make_unique<BossEnemy>();
	mBoss->Init();

	// 互いのポインタを渡す
	mPlayer->SetBoss(mBoss.get());
	mBoss->SetPlayer(mPlayer.get());

	// オブジェクトにCubeMapのテクスチャ番号を渡す
	mPlayer->GetModel()->SetCubeTexture(skybox_->mTextureHandle);
	mPlayer->GetCollider()->GetModel()->SetCubeTexture(skybox_->mTextureHandle);
	mBoss->GetModel()->SetCubeTexture(skybox_->mTextureHandle);
	mBoss->GetCollider()->GetModel()->SetCubeTexture(skybox_->mTextureHandle);
	mPlayer->GetReticle3D()->SetCubeMap(skybox_->mTextureHandle);
	LevelEditor::GetInstance()->SetTextureCubeMap(skybox_->mTextureHandle);


	// パーティクル
	ParticleManager::GetInstance()->Init();
	mDTCParticle = std::make_unique<DiffusionToCircleParticle>();
	mDTCParticle->Init();

}

void GameScene::Update(){

	// シーン切り替え
	// 1キーを押したうえで
	// Bボタンまたは、Enterキーでシーン遷移
	if (mInput->GetPushKey(DIK_1)) {
		if (mInput->GetPused(Gamepad::Button::B) || mInput->GetTriggerKey(DIK_RETURN)) {
			sceneNo = SCENE::RESULT;
		}
	}

	if (mBoss->GetStatus()->HP <= 0.0f || mPlayer->GetStatus()->HP <= 0.0f) {
		sceneNo = SCENE::RESULT;
	}

	// ステータスマネージャ
	StatusManager::GetInstance()->Update();

	// コライダーリストのクリア
	mCollisionManager->ClearColliders();

#ifdef _DEBUG
	skybox_->DrawGUI("Skybox");
	mPlayer->DrawGUI();
	mBoss->DrawGUI();
#endif // _DEBUG
	MainCamera::GetInstance()->Update();

	// スカイボックス
	skybox_->Update();

	mPlayer->Update();
	mBoss->Update();

	// レベルエディタ更新
	LevelEditor::GetInstance()->Update();

	// コライダーリストへの追加処理
	mCollisionManager->SetCollider(mPlayer->GetObject3D()->mCollider);
	mCollisionManager->SetCollider(mBoss->GetObject3D()->mCollider);
	mPlayer->SetColliderListForArrow(mCollisionManager.get());
	mBoss->SetCollider(mCollisionManager.get());

	// 衝突判定を行う
	mCollisionManager->Update();

	// パーティクル
	ParticleManager::GetInstance()->Update();
	mDTCParticle->Update();

}

void GameScene::Draw() {

	// Skyboxの描画前処理
	skybox_->PreDraw();
	//skybox_->Draw();

	// レベルデータ読み込み
	LevelEditor::GetInstance()->Draw();

	// Object3D(3DModel)の描画前処理
	ModelManager::GetInstance()->PreDraw();

	mPlayer->ColliderDraw();
	mBoss->ColliderDraw();

	// Object3D(Skinning)の描画前処理
	ModelManager::GetInstance()->PreDrawForSkinning();

	mPlayer->Draw();
	mBoss->Draw();

	mPlayer->GetReticle3D()->Draw2DReticle();

	//ParticleManager::GetInstance()->PreDraw();
	//mDTCParticle->Draw();

	// 2DSprite(画像)の描画前処理
	SpriteAdministrator::GetInstance()->PreDraw();
	mPlayer->GetStatus()->Draw();
	mBoss->GetStatus()->Draw();

	// ダメージ画像の表記
	DamageReaction::GetInstance()->DrawSprite();

}