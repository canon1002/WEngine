#include "TitleScene.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Editor/LevelEditor.h"
#include "GameEngine/GameBase/Reaction/DamageReaction.h"
#include "GameEngine/Effect/Particle/ParticleManager.h"

void TitleScene::Finalize(){}

//　継承した関数
void TitleScene::Init() {
	// 入力を取得する
	mInput = InputManager::GetInstance();

	mObject = std::make_unique<ShadowObject>();
	mObject->Init("ShadowObject");
	ModelManager::GetInstance()->LoadModel("box", "box.gltf");
	mObject->SetModel("box.gltf");
	mObject->mWorldTransform->rotation = { 0.5f,0.2f,0.0f };

	MainCamera::GetInstance()->Initialize();
	MainCamera::GetInstance()->mWorldTransform->translation.z -= 10;

}

void TitleScene::Update() {
	
	// シーン切り替え
	// Bボタンまたは、Enterキーでシーン遷移
	if (mInput->GetPused(Gamepad::Button::B) || mInput->GetTriggerKey(DIK_RETURN)) {
		sceneNo = SCENE::STAGE;
	}


	MainCamera::GetInstance()->Update();
	mObject->Update();
	mObject->DrawGUI();

}

void TitleScene::Draw(){

	ModelManager::GetInstance()->PreDrawForShadow();
	mObject->Draw();
}
