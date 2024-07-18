#include "TitleScene.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Editor/LevelEditor.h"

void TitleScene::Finalize(){}

//　継承した関数
void TitleScene::Init() {
	// 入力を取得する
	mInput = InputManager::GetInstance();
	// 衝突判定マネージャ
	mCollisionManager = std::make_unique<CollisionManager>();
	
	//testObject_ = ObjectAdministrator::GetInstance()->CreateObject("Resources/objs", "emptyAxis.obj");
	
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

	// SkyBox 読み込み
	DirectXCommon::GetInstance()->srv_->LoadTexture("skybox/rostock_laage_airport_4k.dds");
	

	// skyboxの宣言
	skybox_ = std::make_unique<Skybox>();
	skybox_->Init("skybox","rostock_laage_airport_4k.dds");

	mPlayer = std::make_unique<Player>();
	mPlayer->Init();
	
	mBoss = std::make_unique<BossEnemy>();
	mBoss->Init();
	mBoss->SetPlayer(mPlayer.get());

	// オブジェクトにCubeMapのテクスチャ番号を渡す
	mPlayer->GetModel()->SetCubeTexture(skybox_->mTextureHandle);
	mPlayer->GetCollider()->GetModel()->SetCubeTexture(skybox_->mTextureHandle);
	mBoss->GetModel()->SetCubeTexture(skybox_->mTextureHandle);
	mBoss->GetCollider()->GetModel()->SetCubeTexture(skybox_->mTextureHandle);
	mPlayer->GetReticle3D()->SetCubeMap(skybox_->mTextureHandle);
	LevelEditor::GetInstance()->SetTextureCubeMap(skybox_->mTextureHandle);

	// グリッド生成  // 左の引数はグリッドのセル数、右の引数はセルの大きさを入れる
	grid_ = std::make_unique<Grid3D>(5,1.0f);

	
}

void TitleScene::Update() {
	// コライダーリストのクリア
	mCollisionManager->ClearColliders();

#ifdef _DEBUG
	skybox_->DrawGUI("Skybox");
	mPlayer->DrawGUI();
	mBoss->DrawGUI();
#endif // _DEBUG
	MainCamera::GetInstance()->Update();

	// グリッド
	grid_->Update();

	// スカイボックス
	skybox_->Update();

	mPlayer->Update();
	mBoss->Update();

	// レベルエディタ更新
	LevelEditor::GetInstance()->Update();

	// コライダーリストへの追加処理
	mCollisionManager->SetCollider(mPlayer->GetObject3D()->mCollider);
	mCollisionManager->SetCollider(mBoss->GetObject3D()->mCollider);

	// 衝突判定を行う
	mCollisionManager->Update();

}

void TitleScene::Draw(){


	// Skyboxの描画前処理
	skybox_->PreDraw();
	skybox_->Draw();

	//grid_->PreDraw();
	//grid_->Draw();

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

}
