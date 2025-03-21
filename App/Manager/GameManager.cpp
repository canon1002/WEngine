#include "GameManager.h"
#include "App/Player/Player.h"
#include "App/Enemy/BossEnemy.h"

GameManager* GameManager::instance = nullptr;

GameManager* GameManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new GameManager();
	}
	return instance;
}

void GameManager::Final() {
	// インスタンスのポインタを破棄する
	delete instance;
	instance = nullptr;
}

void GameManager::Init() {

	// 衝突管理クラス
	mCollisionManager = std::make_unique<CollisionManager>();

}

void GameManager::Update() {

	// -- 衝突判定フェーズ -- //

	// コライダーリストの更新を行う
	UpdateColliderList();
	// 衝突判定を行う
	mCollisionManager->Update();
	// コライダーリストのクリア
	mCollisionManager->ClearColliders();

	// -- 衝突後処理フェーズ -- //

	// 敵キャラクターの被弾処理
	if (ChackOnCollided(static_cast<uint32_t>(CollisionTypeId::kEnemy))) {
		
		// 衝突相手のコライダーを検索
		for (std::weak_ptr<GameCollider>collider : mColliderList[static_cast<uint32_t>(CollisionTypeId::kPlayerWeapon)]) {

			// 衝突フラグがfalseだったら次に回す
			if (!collider.lock()->collider->GetOnCollisionFlag()) {
				continue;
			}
			// プレイヤー側の攻撃衝突フラグの切り替え ・ 敵キャラの無敵時間を設定
			mPlayer->Hit();
			mBoss->SetInvincible(1.0f);


			// ヒットストップ発生判定
			 

			// ヒットエフェクトの発生処理

			 
			// ノックバック発生判定

			// ダメージ計算処理
			StatusManager::GetInstance()->ReceiveDamage(mPlayer->GetStatus(),1.0f,mBoss->GetStatus());
		}

	}


	// プレイヤーキャラの被弾処理
	if (ChackOnCollided(static_cast<uint32_t>(CollisionTypeId::kPlayer))) {

		// 衝突相手のコライダーを検索
		for (std::weak_ptr<GameCollider>collider : mColliderList[static_cast<uint32_t>(CollisionTypeId::kEnemyWeapon)]) {

			// 衝突フラグがfalseだったら次に回す
			if (!collider.lock()->collider->GetOnCollisionFlag()) {
				continue;
			}

			// 敵キャラ側の攻撃衝突フラグの切り替え ・ プレイヤーの無敵時間を設定
			mBoss->Hit();
			mPlayer->SetInvincible(1.0f);

			// (ToDo)ジャスト回避判定


			// ヒットストップ発生判定


			// ヒットエフェクトの発生処理


			// ノックバック発生判定

			// ダメージ計算処理
			StatusManager::GetInstance()->ReceiveDamage(mBoss->GetStatus(),1.0f,mPlayer->GetStatus());
		}

		// 衝突相手のコライダーを検索(射撃)
		for (std::weak_ptr<GameCollider>collider : mColliderList[static_cast<uint32_t>(CollisionTypeId::kEnemyBullet)]) {

			// 衝突フラグがfalseだったら次に回す
			if (!collider.lock()->collider->GetOnCollisionFlag()) {
				continue;
			}

			// プレイヤーの無敵時間を設定
			mPlayer->SetInvincible(1.0f);

			// (ToDo)ジャスト回避判定



			// ヒットストップ発生判定


			// ヒットエフェクトの発生処理


			// ノックバック発生判定

			// ダメージ計算処理
			StatusManager::GetInstance()->ReceiveDamage(mBoss->GetStatus(), 1.0f, mPlayer->GetStatus());
		}

	}

}

void GameManager::UpdateColliderList(){

	// コライダーリストのクリア処理
	mColliderList.clear();
	//コライダーリストに 本体コライダー・武器コライダー・弾コライダー を追加する
	mPlayer->SetColliderList();
	mBoss->SetColliderList();

	// 衝突管理マネージャにポインタを渡す

	// 敵キャラのコライダー
	for (std::weak_ptr<GameCollider>collider : mColliderList[static_cast<uint32_t>(CollisionTypeId::kEnemy)]) {
		mCollisionManager->SetCollider(collider.lock()->collider.get());
	}

	// 敵キャラの武器コライダー
	for (std::weak_ptr<GameCollider>collider : mColliderList[static_cast<uint32_t>(CollisionTypeId::kEnemyWeapon)]) {
		// 攻撃処理中ではない場合は追加しない
		if (!mBoss->GetIsOperating()) {
			break;
		}
		mCollisionManager->SetCollider(collider.lock()->collider.get());
	}

	// 敵キャラの弾コライダー
	for (std::weak_ptr<GameCollider>collider : mColliderList[static_cast<uint32_t>(CollisionTypeId::kEnemyBullet)]) {
		mCollisionManager->SetCollider(collider.lock()->collider.get());
	}


	// プレイヤーキャラのコライダー
	for (std::weak_ptr<GameCollider>collider : mColliderList[static_cast<uint32_t>(CollisionTypeId::kPlayer)]) {
		mCollisionManager->SetCollider(collider.lock()->collider.get());
	}

	// プレイヤーキャラの武器コライダー
	for (std::weak_ptr<GameCollider>collider : mColliderList[static_cast<uint32_t>(CollisionTypeId::kPlayerWeapon)]) {
		// 攻撃処理中ではない場合は追加しない
		if (!mPlayer->GetIsOperating()) {
			break;
		}
		mCollisionManager->SetCollider(collider.lock()->collider.get());
	}

}

void GameManager::Draw(){

}


void GameManager::SetCollider(std::shared_ptr<GameCollider> collider) {

	// コライダーの参照を取得
	std::weak_ptr<GameCollider> newCollider = collider;
	// 種別IDを確認
	uint32_t typeID = newCollider.lock()->collider->GetTypeID();
	// 種別IDごとのリストに追加
	mColliderList[typeID].push_back(newCollider);
}

bool GameManager::ChackOnCollided(uint32_t typeID){

	for (std::weak_ptr<GameCollider>collider : mColliderList[typeID]) {
		
		// 衝突していたらtrueを返す
		if (collider.lock()->collider->GetOnCollisionFlag()) {
			return true;
		}

	}

	return false;
}

void GameManager::SetPlayer(Player* player){
	// プレイヤーのポインタを取得する
	mPlayer = player;
}

void GameManager::SetBossEnemy(BossEnemy* boss){
	// ボス敵のポインタを取得する
	mBoss = boss;
}


