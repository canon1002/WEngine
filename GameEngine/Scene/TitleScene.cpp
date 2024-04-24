#include "TitleScene.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"

void TitleScene::Finalize(){}

//　継承した関数
void TitleScene::Init() {
	//testObject_ = ObjectAdministrator::GetInstance()->CreateObject("Resources/objs", "emptyAxis.obj");
	// モデルをセットする
	ModelManager::GetInstance()->LoadModel("plane.gltf");
	ModelManager::GetInstance()->LoadModel("ball.obj");

	// 3dオブジェクトの宣言
	testObject_ = std::make_unique<Object3d>();
	testObject_->Init();
	testObject_->SetModel("plane.gltf");
	testObject_->SetTranslate({ 0.0f,0.0f,10.0f });

}

void TitleScene::Update() {
	testObject_->Update();
}

void TitleScene::Draw(){
	ModelManager::GetInstance()->PreDraw();

	testObject_->Draw();
}
