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
		colliderA->OnCollision();
		colliderB->OnCollision();
	}

}


bool CollisionManager::IsCollision(const AABBCollider& a, const AABBCollider& b){
	
	// AABB同士の判定を行う
	if ((a.GetMin().x <= b.GetMax().x && a.GetMax().x >= b.GetMin().x) &&
		(a.GetMin().y <= b.GetMax().y && a.GetMax().y >= b.GetMin().y) &&
		(a.GetMin().z <= b.GetMax().z && a.GetMax().z >= b.GetMin().z)) {

		return true;

	}

	return false;
}

bool CollisionManager::IsCollision(const SphereCollider& s1, const SphereCollider& s2){

	// 2つの球体の中心点間の距離を求める
	float distance = Length(Subtract(s2.GetWorldPos(), s1.GetWorldPos()));

	// 半径の合計よりも短ければ衝突
	if (distance <= (s1.GetRadius() + s2.GetRadius()))
	{
		return true;
	}

	return false;
}

bool CollisionManager::IsCollision(const AABBCollider& aabb, const SphereCollider& sphere) {

	Vector3 cloosestPoint{
			std::clamp(sphere.GetWorldPos().x,aabb.GetMin().x,aabb.GetMax().x),
			std::clamp(sphere.GetWorldPos().y,aabb.GetMin().y,aabb.GetMax().y),
			std::clamp(sphere.GetWorldPos().z,aabb.GetMin().z,aabb.GetMax().z)
	};

	float distance = Length(Subtract(cloosestPoint, sphere.GetWorldPos()));

	if (distance < sphere.GetRadius()) {
		return true;
	}
	return false;
}
