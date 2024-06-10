#pragma once
#include "GameEngine/Append/Collider/Collider.h"
#include "GameEngine/Append/Transform/WorldTransform.h"

class ObjCollider : public Collider {
public:

	void OnCollision()override;
	void OnCollision(Collider* collider)override;

	// ワールド座標
	inline Vector3 GetWorldPos()override { return mWorldTransform->translation; }
	// ワールド座標のポインタをセットする
	inline void SetWorldPos(const WorldTransform* worldTransform) { mWorldTransform = worldTransform; }

private: 
	// オブジェクトのワールド座標のポインタ
	const WorldTransform* mWorldTransform;

};

