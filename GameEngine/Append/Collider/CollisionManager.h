#pragma once
#include"Collider.h"
#include"CollisionConfig.h"

#include "AABBCollider.h"
#include "OBBCollider.h"
#include "RayCollider.h"
#include "SphereCollider.h"
#include "PlaneCollider.h"

class CollisionManager 
{
public:

	CollisionManager();
	~CollisionManager();
	void Init();
	void Update();
	void Draw();

	void SetCollider(Collider* collider) { m_colliders.push_back(collider); }
	void ClearColliders() { m_colliders.clear(); }
	
	void CheckAllCollisions();

private:

	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);


	bool IsCollision(const AABBCollider& a, const AABBCollider& b);

	bool IsCollision(const SphereCollider& s1, const SphereCollider& s2);


	bool IsCollision(const AABBCollider& aabb, const SphereCollider& sphere);

private:

	// コライダーリスト
	std::list<Collider*> m_colliders;

};
