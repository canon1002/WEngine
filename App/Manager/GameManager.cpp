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

	// ステータス管理クラス
	//StatusManager::GetInstance()->Init();

}

void GameManager::Update() {

	// -- 衝突判定フェーズ -- //

	// コライダーリストの更新を行う
	UpdateColliderList();
	

	// -- 衝突後処理フェーズ -- //

	// 衝突判定と衝突後の処理を行う
	UpdateCollisionManager();

	// -- ステータス更新 -- //

	// 各キャラクターのステータスとUIの更新を行う
	UpdateStatusManager();

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

void GameManager::UpdateCollisionManager(){

	// 衝突判定を行う
	mCollisionManager->Update();
	// コライダーリストのクリア
	mCollisionManager->ClearColliders();

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
			mBoss->SetInvincible(0.8f);


			// ヒットストップ発生判定
			mIsRequestHitStop = true;
			mHitStopDuration = 0.1f;

			// ヒットエフェクトの発生処理


			// ノックバック発生判定


			// ダメージ計算処理
			StatusManager::GetInstance()->ReceiveDamage(mPlayer->GetStatus(), 1.0f, mBoss->GetStatus());
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
			mPlayer->SetInvincible(0.8f);

			// (ToDo)ジャスト回避判定


			// ヒットストップ発生判定


			// ヒットエフェクトの発生処理


			// ノックバック発生判定


			// ダメージ計算処理
			StatusManager::GetInstance()->ReceiveDamage(mBoss->GetStatus(), 1.0f, mPlayer->GetStatus());
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

void GameManager::UpdateStatusManager(){

	// ステータス管理クラス 更新
	StatusManager::GetInstance()->Update();

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

void GameManager::ResolveCollision(){

	// 簡易衝突判定(プレイヤーとボス敵)
	float dx = mPlayer->GetWorldPos().x - mBoss->GetWorldPos().x;
	float dz = mPlayer->GetWorldPos().z - mBoss->GetWorldPos().z;
	float distance = dx * dx + dz * dz;
	float radiusSum = (mPlayer->GetObject3D()->GetWorldTransform()->scale.x / 2.0f) +
		(mBoss->GetObject3D()->GetWorldTransform()->scale.x / 2.0f);

	// 非衝突時は以下の押し出し処理をスキップする
	if (distance > (radiusSum * radiusSum)) {
		return;
	}

	// 平方根
	distance = std::sqrt(distance);

	// 0除算避けのチェック
	if (distance == 0.0f) {
		// 小さな値を設定して適当な方向を与える
		dx = 0.01f;
		dz = 0.01f;
		distance = std::sqrt(dx * dx + dz * dz);
	}

	float overlap = distance - (mPlayer->GetObject3D()->GetWorldTransform()->scale.x / 2.0f) - 
		(mBoss->GetObject3D()->GetWorldTransform()->scale.x / 2.0f);

	// プレイヤーを押し出す
	mPlayer->GetObject3D()->mWorldTransform->translation.x -= overlap * (dx / distance);
	mPlayer->GetObject3D()->mWorldTransform->translation.z -= overlap * (dz / distance);

}


