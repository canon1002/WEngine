#pragma once
#include"Collider.h"
#include"CollisionConfig.h"

class CollisionManager 
{
public:

	CollisionManager();
	~CollisionManager();
	void Initialize();
	void Update();
	void Draw();

	void SetCollider(Collider* collider) { m_colliders.push_back(collider); }
	void ClearColliders() { m_colliders.clear(); }
	
	void CheckAllCollisions();

private:

	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

private:

	// コライダーリスト
	std::list<Collider*> m_colliders;

};
