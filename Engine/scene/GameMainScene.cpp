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

	// 衝突判定マネージャー
	cManager = std::make_unique<CollisionManager>();
	cManager->Initialize();

	// プレイヤーの宣言
	player_ = std::make_unique<Player>();
	player_->Init();

	// プレイヤーの宣言
	enemy_ = std::make_unique<Enemy>();
	enemy_->Init();

	// エネミーの生存人数
	aliveEnemyCount_ = 1;

	// 天球
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	// レールカメラ
	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Initialize();
	railCamera_->SetTranslate({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-48.0f} });

}

void GameMainScene::Update() {
	// get command
	// Initializeにてコマンドをあーだこーだしておくこと
	command_ = inputHandler_->HandleInput();

	// set command
	if (this->command_) {
		command_->Exec(*player_);
	}

	if (aliveEnemyCount_ > 0) {

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
		//for (const auto& bullet : playerBullets_) {
		//	// Bulletが消失条件を満たす場合
		//	if (bullet->GetIsActive() == false) {
		//		// ここでbulletを削除
		//	}
		//	else {
		//		bullet->Update();
		//	}
		//}

		auto it = playerBullets_.begin();  // イテレータを初期化

		while (it != playerBullets_.end()) {
			const auto& bullet = *it;

			// Bulletが消失条件を満たす場合
			if (!bullet->GetIsActive()) {
				it = playerBullets_.erase(it);  // erase()は削除された要素の次の有効なイテレータを返す
			}
			else {
				bullet->Update();
				++it;  // イテレータを次に進める
			}
		}

		// エネミー 更新
		enemy_->Update();
		if (enemy_->GetIsActive() == false) {
			aliveEnemyCount_--;
		}

		// レールカメラ 更新
/*		float newZ = railCamera_->GetTranslate().z + 0.02f;
		railCamera_->SetTranslate(Vec3{ railCamera_->GetTranslate().x,
			railCamera_->GetTranslate().y,newZ })*/;
		railCamera_->Update();

		// レールカメラとメインカメラの切り替え
		if (Input::GetInstance()->GetTriggerKey(DIK_9)) {
			if (!isActiveRailCamera_) {
				isActiveRailCamera_ = true;
				player_->SetCamera(railCamera_.get());
				enemy_->SetCamera(railCamera_.get());
				skydome_->SetCamera(railCamera_.get());
				for (const auto& bullet : playerBullets_) {
					bullet->SetCamera(railCamera_.get());
				}
			}
			else if (isActiveRailCamera_) {
				isActiveRailCamera_ = false;
				player_->SetCamera(MainCamera::GetInstance());
				enemy_->SetCamera(MainCamera::GetInstance());
				skydome_->SetCamera(MainCamera::GetInstance());
				for (const auto& bullet : playerBullets_) {
					bullet->SetCamera(MainCamera::GetInstance());
				}
			}
		}

		///////////////////////////////////////////////////////////////
		/// 衝突判定
		///////////////////////////////////////////////////////////////

		// 衝突マネージャーのリストをクリア
		cManager->ClearColliders();

		// コライダーを衝突マネージャーのリストに登録
		cManager->SetCollider(player_.get());
		cManager->SetCollider(enemy_.get());
		//for (Enemy* m_enemy : m_enemys) {
		//	cManager->SetCollider(m_enemy);
		//}
		for (const auto& pBullet : playerBullets_) {
			cManager->SetCollider(pBullet.get());
		}
		/*for (EnemyBullet* enemyBullet : m_enemyBullets) {
			cManager->SetCollider(enemyBullet);
		}*/



		cManager->Update();

	}
	else if (aliveEnemyCount_ <= 0) {
		// メインゲームに切り替え
		IScene::sceneNo = CLEAR;
	}
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

	// エネミー 描画
	enemy_->Draw();

}
