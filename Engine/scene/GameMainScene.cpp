#include "GameMainScene.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Object/Sprite/SpriteCommon.h"
#include <cassert>
#include <fstream>

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
	//enemys_ = std::make_unique<Enemy>();
	//enemys_->Init();

	// エネミーの生存人数
	aliveEnemyCount_ = 1;

	// 天球
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	// レールカメラ
	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Initialize();
	railCamera_->SetTranslate({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-48.0f} });

	LoadEnemyPopDate();

}

void GameMainScene::Update() {

	// スペースキー入力でシーン切り替え
	if (InputManager::GetInstance()->GetKey()->GetTriggerKey(DIK_SPACE)) {
		sceneNo = TITLE;
	}

	// 敵発生コマンドの更新を行う
	UpdateEnemyPopCommands();

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
		if (input_->GetKey()->GetTriggerKey(DIK_RETURN)||
			Gamepad::getTriger(Gamepad::Triger::RIGHT)
			) {

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

		// エネミーの弾の発射 & 更新処理
		auto itE = enemies_.begin();  // イテレータを初期化
		while (itE != enemies_.end()) {
			const auto& enemy = *itE;
			
			// 消失条件を満たす場合
			if (!enemy->GetIsActive()) {
				itE = enemies_.erase(itE);  // erase()は削除された要素の次の有効なイテレータを返す
				//aliveEnemyCount_--;
			}
			else {
			
				// 弾の発射
				if (enemy->GetIsShotBullet()) {

					std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
					if (isActiveRailCamera_) {

						newBullet->Init(enemy.get(), player_.get());
						newBullet->SetCamera(railCamera_.get());
					}
					else {
						newBullet->Init(enemy.get(), player_.get());
					}

					enemyBullets_.push_back(std::move(newBullet));
					enemy->SetCoolDown();
				}

				enemy->Update();
				++itE;  // イテレータを次に進める
			}

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
				skydome_->SetCamera(railCamera_.get());
				for (const auto& enemy : enemies_) {
					enemy->SetCamera(railCamera_.get());
				}
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
				skydome_->SetCamera(MainCamera::GetInstance());
				for (const auto& enemy : enemies_) {
					enemy->SetCamera(MainCamera::GetInstance());
				}
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
		for (const auto& enemy : enemies_) {
			cManager->SetCollider(enemy.get());
		}
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
	for (const auto& enemy : enemies_) {
		enemy->Draw();
	}
	// プレイヤーの弾 -- 描画 --
	for (const auto& bullet : enemyBullets_) {
		bullet->Draw();
	}

	SpriteCommon::GetInstance()->DrawBegin();
	player_->GetReticle()->Draw();

}

// 敵発生データの読み込み
void GameMainScene::LoadEnemyPopDate()
{
	// ファイルを開く
	std::ifstream file;
	file.open("./Resources/csv/enemyPop.csv");
	assert(file.is_open());

	// ファイルの内容をストリームにコピー
	enemyPopCommands << file.rdbuf();

	// ファイルを閉じる
	file.close();

}


void GameMainScene::UpdateEnemyPopCommands()
{
	// 待機処理
	if (m_isWait)
	{
		m_waitTime--;
		if (m_waitTime <= 0)
		{
			m_isWait = false;
		}
		return;
	}

	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyPopCommands, line))
	{
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		// ,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0)
		{
			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0)
		{
			//ｘ座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 敵を発生させる
			std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
			newEnemy->Init(Vec3(x, y, z));
			// 敵キャラにゲームシーンとプレイヤーを渡す
			//newEnemy->SetGameScene(this);
			//newEnemy->SetPlayer(m_player);
			// 敵キャラをリストに登録する
			enemies_.push_back(std::move(newEnemy));

		}

		// WAITコマンド
		else if (word.find("WAIT") == 0)
		{
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待機開始
			m_isWait = true;
			m_waitTime = waitTime;

			// コマンドループを抜ける
			break;
		}
	}
}
