#include "TitleScene.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"

void TitleScene::Finalize(){}

//　継承した関数
void TitleScene::Init() {
	//testObject_ = ObjectAdministrator::GetInstance()->CreateObject("Resources/objs", "emptyAxis.obj");
	// モデルをセットする
	ModelManager::GetInstance()->LoadModel("plane", "plane.gltf");
	ModelManager::GetInstance()->LoadModel("ball", "ball.obj");
	DirectXCommon::GetInstance()->srv_->LoadTexture("skybox/UI01.dds");

	// 3dオブジェクトの宣言
	testObject_ = std::make_unique<Object3d>();
	testObject_->Init();
	testObject_->SetModel("ball.obj");
	testObject_->SetTranslate({ 0.0f,0.0f,10.0f });
	
	// skyboxの宣言
	skybox_ = std::make_unique<Skybox>();
	skybox_->Init("skybox","UI01.dds");

}

void TitleScene::Update() {
#ifdef _DEBUG
	skybox_->DrawGUI("Skybox");
#endif // _DEBUG

	testObject_->Update();
	skybox_->Update();
}

void TitleScene::Draw(){

	// Skyboxの描画前処理
	skybox_->PreDraw();

	skybox_->Draw();

	// Object3D(3DModel)の描画前処理
	ModelManager::GetInstance()->PreDraw();

	testObject_->Draw();
}
