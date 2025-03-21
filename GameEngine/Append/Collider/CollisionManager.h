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
public: // -- 公開 メンバ関数 -- //


	CollisionManager();
	~CollisionManager();

	void Init();
	void Update();
	void Draw();

	// 衝突判定を行うコライダーをリストにセットする
	void SetCollider(Collider* collider) { mColliders.push_back(collider); }
	// コライダーリストのクリア
	void ClearColliders() { mColliders.clear(); }
	
	// リスト内のコライダーの衝突判定を行う	
	void CheckAllCollisions();

	// コライダーが衝突可能な属性か判定する
	bool CheckColliderType(Collider* a, Collider* b);

private: // -- 非公開 メンバ関数 -- //

	// コライダー同士の衝突判定を行う
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

private:// -- 非公開 メンバ変数 -- //

	// コライダーリスト
	std::list<Collider*> mColliders;

};
