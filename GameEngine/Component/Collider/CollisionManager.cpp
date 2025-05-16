#include "CollisionManager.h"

CollisionManager::CollisionManager() { this->Init(); }

CollisionManager::~CollisionManager() {}

void CollisionManager::Init() {}

void CollisionManager::Update() { this->CheckAllCollisions(); }

void CollisionManager::Draw() {}

void CollisionManager::CheckAllCollisions() {

	// リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = mColliders.begin();
	for (; itrA != mColliders.end(); ++itrA) {

		// イテレーターからコライダーを取得
		Collider* colliderA = (*itrA);

		// このイテレーターは前のイテレーターの次の要素から回す
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != mColliders.end(); ++itrB) {

			// イテレーターからコライダーを取得
			Collider* colliderB = (*itrB);

			// 当たり判定
			CheckCollisionPair(colliderA, colliderB);

		}
	}
}

bool CollisionManager::CheckColliderType(Collider* a, Collider* b){

	switch (a->GetTypeID())
	{
	case static_cast<uint32_t>(CollisionTypeId::kDefault):
		break;

		// 敵キャラ
	case static_cast<uint32_t>(CollisionTypeId::kEnemy):
		
		if (b->GetTypeID() == static_cast<uint32_t>(CollisionTypeId::kPlayerWeapon)) {
			return true;
		}

		break;

		// 敵キャラ - 武器
	case static_cast<uint32_t>(CollisionTypeId::kEnemyWeapon):

		if (b->GetTypeID() == static_cast<uint32_t>(CollisionTypeId::kPlayer)) {
			return true;
		}

		break;
		
		// 敵キャラ - 弾
	case static_cast<uint32_t>(CollisionTypeId::kEnemyBullet):

		if (b->GetTypeID() == static_cast<uint32_t>(CollisionTypeId::kPlayer)) {
			return true;
		}

		break;

		// プレイヤーキャラ
	case static_cast<uint32_t>(CollisionTypeId::kPlayer):

		if (b->GetTypeID() == static_cast<uint32_t>(CollisionTypeId::kEnemyWeapon)) {
			return true;
		}

		break;

		// プレイヤーキャラ - 武器
	case static_cast<uint32_t>(CollisionTypeId::kPlayerWeapon):

		if (b->GetTypeID() == static_cast<uint32_t>(CollisionTypeId::kEnemy)) {
			return true;
		}

		break;

		// プレイヤーキャラ - 弾
	case static_cast<uint32_t>(CollisionTypeId::kPlayerBullet):

		if (b->GetTypeID() == static_cast<uint32_t>(CollisionTypeId::kEnemy)) {
			return true;
		}

		break;
	default:
		break;
	}

	// 上記の条件に引っかからない場合は衝突しない
	return false;
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {

	// 衝突フィルタリング
	if (!CheckColliderType(colliderA,colliderB)) {
		return;
	}

	// 衝突判定を行う
	if (colliderA->IsCollision(colliderB)) {
		// 衝突していたら衝突時の処理を呼び出す
		colliderA->OnCollision(colliderB);
		colliderB->OnCollision(colliderA);
	}

}