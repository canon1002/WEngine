#include "TitleScene.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"

void TitleScene::Finalize(){}

//　継承した関数
void TitleScene::Init() {
	testObject_ = ObjectAdministrator::GetInstance()->CreateObject("Resources/objs", "axis.obj");
}

void TitleScene::Update() {
	MainCamera* camera = MainCamera::GetInstance();
	testObject_->Update(camera);
}

void TitleScene::Draw(){
	testObject_->Draw();
}
