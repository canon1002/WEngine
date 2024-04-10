#include "GameMainScene.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Object/Sprite/SpriteCommon.h"

void GameMainScene::Finalize()
{
	delete command_;
}

//　継承した関数
void GameMainScene::Init() {

	input_ = InputManager::GetInstance();
	camera_ = MainCamera::GetInstance();
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

	// スペースキー入力でシーン切り替え
	if (InputManager::GetInstance()->GetKey()->GetTriggerKey(DIK_SPACE)) {
		sceneNo = TITLE;
	}

	// get command
	// Initializeにてコマンドをあーだこーだしておくこと
	command_ = inputHandler_->HandleInput();
	// set command
	if (this->command_) {
		command_->Exec(*player_);
	}

	// 敵が生存していたらループし続ける
	if (aliveEnemyCount_ > 0) {

		// プレイヤーの弾の発射(後日コマンドに移行)
		if (input_->GetKey()->GetTriggerKey(DIK_RETURN)) {

			std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
			if (isActiveRailCamera_) {

				newBullet->Init(player_.get());
				newBullet->SetCamera(railCamera_.get());
			}
			else {
				newBullet->Init(player_.get());
			}

			playerBullets_.push_back(std::move(newBullet));
		}

		// エネミーの弾の発射
		if (enemy_->GetIsShotBullet()) {

			std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
			if (isActiveRailCamera_) {

				newBullet->Init(enemy_.get(),player_.get());
				newBullet->SetCamera(railCamera_.get());
			}
			else {
				newBullet->Init(enemy_.get(), player_.get());
			}

			enemyBullets_.push_back(std::move(newBullet));
			enemy_->SetCoolDown();
		}

		// 天球
		skydome_->Update();

		// プレイヤー -- 更新 --
		if (input_->GetKey()->GetPushKey(DIK_Q)) { player_; }
		if (input_->GetKey()->GetPushKey(DIK_Q)) { player_; }
		player_->Update();

		// プレイヤーの弾 -- 更新 --
		auto itPB = playerBullets_.begin();  // イテレータを初期化
		while (itPB != playerBullets_.end()) {
			const auto& bullet = *itPB;
			// Bulletが消失条件を満たす場合
			if (!bullet->GetIsActive()) {
				itPB = playerBullets_.erase(itPB);  // erase()は削除された要素の次の有効なイテレータを返す
			}
			else {
				bullet->Update();
				++itPB;  // イテレータを次に進める
			}
		}

		// エネミー 更新
		enemy_->Update();
		if (enemy_->GetIsActive() == false) {
			//aliveEnemyCount_--;
		}
		// エネミーの弾 -- 更新 --
		auto itEB = enemyBullets_.begin();  // イテレータを初期化
		while (itEB != enemyBullets_.end()) {
			const auto& bullet = *itEB;
			// Bulletが消失条件を満たす場合
			if (!bullet->GetIsActive()) {
				itEB = enemyBullets_.erase(itEB);  // erase()は削除された要素の次の有効なイテレータを返す
			}
			else {
				bullet->Update();
				++itEB;  // イテレータを次に進める
			}
		}


		// レールカメラ 更新
/*		float newZ = railCamera_->GetTranslate().z + 0.02f;
		railCamera_->SetTranslate(Vec3{ railCamera_->GetTranslate().x,
			railCamera_->GetTranslate().y,newZ })*/;
		railCamera_->Update();

		// レールカメラとメインカメラの切り替え
		if (input_->GetKey()->GetTriggerKey(DIK_9)) {
			if (!isActiveRailCamera_) {
				isActiveRailCamera_ = true;
				player_->SetCamera(railCamera_.get());
				enemy_->SetCamera(railCamera_.get());
				skydome_->SetCamera(railCamera_.get());
				for (const auto& bullet : playerBullets_) {
					bullet->SetCamera(railCamera_.get());
				}
				for (const auto& bullet : enemyBullets_) {
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
				for (const auto& bullet : enemyBullets_) {
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
		for (const auto& enemyBullet : enemyBullets_) {
			cManager->SetCollider(enemyBullet.get());
		}

		// 衝突判定を行う
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
	// プレイヤーの弾 -- 描画 --
	for (const auto& bullet : enemyBullets_) {
		bullet->Draw();
	}

	SpriteCommon::GetInstance()->DrawBegin();
	player_->GetReticle()->Draw();

}
