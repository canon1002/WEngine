#include "GameMainScene.h"
#include "../base/DirectXCommon.h"
#include "../object/camera/MainCamera.h"
#include "../InputManager.h"

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
	enemy1_ = std::make_unique<Enemy>();
	enemy1_->Init({ -5.0f,0.0f,40.0f });
	enemy2_ = std::make_unique<Enemy>();
	enemy2_->Init({ 5.0f,0.0f,40.0f });

	// エネミーの生存人数
	aliveEnemyCount_ = 3;

	// 天球
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	// レールカメラ
	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Initialize();
	railCamera_->SetTranslate({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} });
	
	isActiveRailCamera_ = true;
	player_->SetCamera(railCamera_.get());
	enemy_->SetCamera(railCamera_.get());
	enemy1_->SetCamera(railCamera_.get());
	enemy2_->SetCamera(railCamera_.get());
	skydome_->SetCamera(railCamera_.get());
	for (const auto& bullet : playerBullets_) {
		bullet->SetCamera(railCamera_.get());
	}

	// プレイヤーの弾 -- 更新 --
	auto it = playerBullets_.begin();  // イテレータを初期化

	while (it != playerBullets_.end()) {
		const auto& bullet = *it;
		it = playerBullets_.erase(it);  // erase()は削除された要素の次の有効なイテレータを返す
	}

	// 弾の上限
	bulletSaveMax = 3;
	bulletSaveCount = 3;

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize();
	sprite_->SetTexture("Resources/texture/uvChecker.png");
	sprite_->SetTextureSize({ 64.0f,64.0f });
	sprite_->SetSpriteSize({ 1280.0f,720.0f });
	sprite_->SetColor({ 0.0f,0.0f,0.0f,0.0f });

	// シーン切り替え用
	alpth_ = 0.0f;
	isSceneChange = false;
	fadeTimer_ = 60;
}

void GameMainScene::Update() {
	// get command
	// Initializeにてコマンドをあーだこーだしておくこと
	command_ = inputHandler_->HandleInput();

	// set command
	if (this->command_) {
		command_->Exec(*player_);
	}

	sprite_->Update();

	if (aliveEnemyCount_ <= 0) {
		isSceneChange = true;
	}

	if (aliveEnemyCount_ > 0) {

		// 弾の発射(後日コマンドに移行)
		if (InputManager::GetInstance()->GetKey()->GetTriggerKey(DIK_RETURN) ||
			Gamepad::getTriger(Gamepad::Triger::RIGHT)
			) {

			std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
			Vec3 bulletVel = player_->GetReticleAxis();

			newBullet->Init(player_->GetWorld().translate, bulletVel);
			newBullet->SetCamera(railCamera_.get());

			playerBullets_.push_back(std::move(newBullet));
		}

		// 天球
		skydome_->Update();

		// プレイヤー -- 更新 --
		player_->Update();

		// プレイヤーの弾 -- 更新 --
		auto it = playerBullets_.begin();  // イテレータを初期化

		while (it != playerBullets_.end()) {
			const auto& bullet = *it;

			// Bulletが消失条件を満たす場合
			if (bullet->GetIsActive() == false) {
				it = playerBullets_.erase(it);  // erase()は削除された要素の次の有効なイテレータを返す
			}
			else {
				bullet->Update();
				++it;  // イテレータを次に進める
			}
		}

		// エネミー 更新
		enemy_->Update();
		enemy1_->Update();
		enemy2_->Update();
		if (enemy_->GetIsActive() == false &&
			enemy1_->GetIsActive() == false &&
			enemy2_->GetIsActive() == false) {
			aliveEnemyCount_--;
		}

		// レールカメラ 更新
		railCamera_->Update();


		///////////////////////////////////////////////////////////////
		/// 衝突判定
		///////////////////////////////////////////////////////////////

		// 衝突マネージャーのリストをクリア
		cManager->ClearColliders();

		// コライダーを衝突マネージャーのリストに登録
		cManager->SetCollider(player_.get());
		cManager->SetCollider(enemy_.get());
		cManager->SetCollider(enemy1_.get());
		cManager->SetCollider(enemy2_.get());
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
	else if (isSceneChange == true) {
		if (alpth_ < 1.0f) {
			alpth_ += 0.05f;
		}
		sprite_->SetColor({ 0.0f,0.0f,0.0f,alpth_ });

		if (alpth_ >= 1.0f) {

			sceneNo = CLEAR;
		}
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
	enemy1_->Draw();
	enemy2_->Draw();

	// 2dSpriteの描画準備
	SpriteCommon::GetInstance()->DrawBegin();
	player_->GetReticle()->Draw();
	sprite_->Draw();
}
