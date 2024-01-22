#include "GameMainScene.h"
#include "../base/DirectXCommon.h"

//　継承した関数
void GameMainScene::Init() {

	// プレイヤーの宣言
	player_ = std::make_unique<Player>();
	player_->Init();
}

void GameMainScene::Update() {
	player_->Update();
	ImGui::Text("GameScene");
}

void GameMainScene::Draw() {
	player_->Draw();
}
