#include "GameMainScene.h"
#include "../base/DirectXCommon.h"
#include "../object/camera/MainCamera.h"
#include "../Input.h"

void GameMainScene::Finalize()
{

}

//　継承した関数
void GameMainScene::Init() {

	inputHandler_ = std::make_unique<InputHandle>();

	// Assign command
	inputHandler_->AssingnMoveUpCommand2PressKeyW();
	inputHandler_->AssingnMoveUpCommand2PressKeyWA();
	inputHandler_->AssingnMoveUpCommand2PressKeyWD();
	inputHandler_->AssingnMoveDownCommand2PressKeyS();
	inputHandler_->AssingnMoveDownCommand2PressKeySA();
	inputHandler_->AssingnMoveDownCommand2PressKeySD();
	inputHandler_->AssingnMoveLeftCommand2PressKeyA();
	inputHandler_->AssingnMoveRightCommand2PressKeyD();

	// プレイヤーの宣言
	player_ = std::make_unique<Player>();
	player_->Init();

	// 天球
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	// レールカメラ
	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Initialize();
	railCamera_->SetTranslate({ {1.0f,1.0f,1.0f},{0.314f,0.0f,0.0f},{0.0f,16.0f,-48.0f} });

}

void GameMainScene::Update() {
	// get command
	// Initializeにてコマンドをあーだこーだしておくこと
	command_ = inputHandler_->HandleInput();

	// set command
	if (this->command_) {
		command_->Exec(*player_);
	}

	// 弾の発射(後日コマンドに移行)
	if (Input::GetInstance()->GetTriggerKey(DIK_RETURN)) {
		
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		if (isActiveRailCamera_) {
			newBullet->Init(player_->GetWorldRailT());
			newBullet->SetCamera(railCamera_.get());
		}
		else {
			newBullet->Init(player_->GetWorld().translate);
		}

		playerBullets_.push_back(std::move(newBullet));
	}

	// 天球
	skydome_->Update();

	// プレイヤー -- 更新 --
	player_->Update();
	// プレイヤーの弾 -- 更新 --
	for (const auto& bullet : playerBullets_) {
		bullet->Update();
	}

	// レールカメラ 更新
	/*float newY = railCamera_->GetRotate().y + 0.02f;
	railCamera_->SetRotate(Vec3{ railCamera_->GetRotate().x,
		newY,railCamera_->GetRotate().z });*/
	float newZ = railCamera_->GetTranslate().z + 0.02f;
	railCamera_->SetTranslate(Vec3{ railCamera_->GetTranslate().x,
		railCamera_->GetTranslate().y,newZ });
	railCamera_->Update();

	// レールカメラとメインカメラの切り替え
	if (Input::GetInstance()->GetTriggerKey(DIK_9)) {
		if (!isActiveRailCamera_) {
			isActiveRailCamera_ = true;
			player_->SetCamera(railCamera_.get());
			skydome_->SetCamera(railCamera_.get());
			for (const auto& bullet : playerBullets_) {
				bullet->SetCamera(railCamera_.get());
			}
		}
		else if (isActiveRailCamera_) {
			isActiveRailCamera_ = false;
			player_->SetCamera(MainCamera::GetInstance());
			skydome_->SetCamera(MainCamera::GetInstance());
			for (const auto& bullet : playerBullets_) {
				bullet->SetCamera(MainCamera::GetInstance());
			}
		}
	}

	ImGui::Text("GameScene");
}

void GameMainScene::Draw() {
	// 天球
	skydome_->Draw();

	// プレイヤー -- 描画 --
	player_->Draw();
	// プレイヤーの弾 -- 描画 --
	for (const auto& bullet : playerBullets_) {
		bullet->Draw();
	}
}
