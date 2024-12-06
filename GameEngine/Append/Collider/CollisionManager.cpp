#include "CollisionManager.h"

CollisionManager::CollisionManager() { this->Init(); }

CollisionManager::~CollisionManager() {}

void CollisionManager::Init() {}

void CollisionManager::Update() { this->CheckAllCollisions(); }

void CollisionManager::Draw() {}

void CollisionManager::CheckAllCollisions() {

	// リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = m_colliders.begin();
	for (; itrA != m_colliders.end(); ++itrA) {

		// イテレーターからコライダーを取得
		Collider* colliderA = (*itrA);

		// このイテレーターは前のイテレーターの次の要素から回す
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != m_colliders.end(); ++itrB) {

			// イテレーターからコライダーを取得
			Collider* colliderB = (*itrB);

			// 当たり判定
			CheckCollisionPair(colliderA, colliderB);

		}
	}
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {

	// 衝突フィルタリング
	if ((colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask()) == 0 ||
	    (colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask()) == 0) {
		return;
	}

	// 衝突判定を行う
	if (colliderA->IsCollision(colliderB)) {
		// 衝突していたら衝突時の処理を呼び出す
		colliderA->OnCollision(colliderB);
		colliderB->OnCollision(colliderA);
	}

}