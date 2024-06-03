#include "CollisionManager.h"

CollisionManager::CollisionManager() { this->Initialize(); }

CollisionManager::~CollisionManager() {}

void CollisionManager::Initialize() {}

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

	// コライダーのワールド座標を取得
	Vector3 posA = colliderA->GetWorldPos();
	float radA = colliderA->GetRadius();
	Vector3 posB = colliderB->GetWorldPos();
	float radB = colliderB->GetRadius();

	// 距離の判定
	if ((posA.x + radA) > (posB.x - radB) && (posA.x - radA) < (posB.x + radB) &&
	    (posA.y + radA) > (posB.y - radB) && (posA.y - radA) < (posB.y + radB) &&
	    (posA.z + radA) > (posB.z - radB) && (posA.z - radA) < (posB.z + radB)) {

		colliderA->OnCollision(colliderB);
		colliderB->OnCollision(colliderA);

		// エフェクトを生成し、初期化
		// Effect* newEffect = new Effect();
		// newEffect->Initialize(EFFECT_TYPE::DISCRETE, { quadA.rightTop.x - (quadA.rightTop.x -
		// quadA.leftTop.x),quadA.leftTop.y });

		// エフェクトを登録
		// m_effects.push_back(newEffect);
	}
}


bool CollisionManager::IsCollision(const AABB& a, const AABB& b) {

	if ((a.min.x <= b.max.x && a.max.x >= b.min.x) &&
		(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
		(a.min.z <= b.max.z && a.max.z >= b.min.z)) {

		return true;

	}

	return false;

}

bool CollisionManager::IsCollision(const OBB& a, const OBB& b)
{
	a, b;
	return false;
}
