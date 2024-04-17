#include "TitleScene.h"
#include "GameEngine/Object/Model/ModelManager.h"

void TitleScene::Finalize(){}

//　継承した関数
void TitleScene::Init() {
	testObject_ = std::make_unique<Object3d>();
	
}

void TitleScene::Update() {}

void TitleScene::Draw(){}
