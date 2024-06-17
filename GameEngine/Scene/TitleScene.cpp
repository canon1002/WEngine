#include "TitleScene.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

void TitleScene::Finalize(){}

//　継承した関数
void TitleScene::Init() {
	// 入力を取得する
	mInput = InputManager::GetInstance();
	
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

	// SkyBox 読み込み
	DirectXCommon::GetInstance()->srv_->LoadTexture("skybox/rostock_laage_airport_4k.dds");

	// 3dオブジェクトの宣言
	testObject_ = std::make_unique<Object3d>();
	testObject_->Init("TestObj");
	testObject_->SetModel("sneakWalk.gltf");
	testObject_->GetModel()->skinning_ = new Skinnig();
	testObject_->GetModel()->skinning_->Init("human", "walk.gltf",
		testObject_->GetModel()->modelData);
	testObject_->SetTranslate({ 0.0f,0.0f,5.0f });
	
	
	testObject02_ = std::make_unique<Object3d>("Test Plane");
	testObject02_->Init("Test Plane");
	testObject02_->SetModel("Bow.obj");
	testObject02_->SetScale({ 1.0f,1.0f,1.0f });
	testObject02_->SetRotate({ 0.0f,0.0f,0.0f });
	testObject02_->SetTranslate({ 2.0f,0.0f,7.5f });

	// skyboxの宣言
	skybox_ = std::make_unique<Skybox>();
	skybox_->Init("skybox","rostock_laage_airport_4k.dds");

	mPlayer = std::make_unique<Player>();
	mPlayer->Init();

	// オブジェクトにCubeMapのテクスチャ番号を渡す
	testObject_->GetModel()->SetCubeTexture(skybox_->mTextureHandle);
	testObject02_->GetModel()->SetCubeTexture(skybox_->mTextureHandle);
	mPlayer->GetModel()->SetCubeTexture(skybox_->mTextureHandle);

	// グリッド生成  // 左の引数はグリッドのセル数、右の引数はセルの大きさを入れる
	grid_ = std::make_unique<Grid3D>(5,1.0f);

	
}

void TitleScene::Update() {
#ifdef _DEBUG
	skybox_->DrawGUI("Skybox");
	testObject_->DrawGUI();
	mPlayer->DrawGUI();
	testObject02_->DrawGUI();
#endif // _DEBUG
	MainCamera::GetInstance()->Update();

	// グリッド
	grid_->Update();

	// SkinningModel 忍び歩き
	testObject_->Update();

	// スカイボックス
	skybox_->Update();

	mPlayer->Update();

	// obj
	testObject02_->mWorldTransform->SetParent(mPlayer->GetModel()->skinning_->GetSkeleton().joints.at(35).transform.GetAffinMatrix());
	testObject02_->Update();


}

void TitleScene::Draw(){


	// Skyboxの描画前処理
	skybox_->PreDraw();
	skybox_->Draw();

	//grid_->PreDraw();
	//grid_->Draw();

	// Object3D(3DModel)の描画前処理
	ModelManager::GetInstance()->PreDraw();

	testObject02_->Draw();

	// Object3D(Skinning)の描画前処理
	ModelManager::GetInstance()->PreDrawForSkinning();

	testObject_->Draw();
	mPlayer->Draw();

}
