#pragma once
#include "GameEngine/Math/Math.h"
#include "CollisionConfig.h"
#include <list>

/// <summary>
///	コライダー
/// <summary>
class Collider
{
public:

	// 純粋仮想関数
	virtual void OnCollision() = 0;
	virtual void OnCollision(Collider* collider) = 0;
	
	// ワールド座標
	virtual Vector3 GetWorldPos() = 0;
	
	// 半径
	void SetRadius(float radius) { m_radius = radius; }
	float GetRadius() { return m_radius; }

	// コライダー
	virtual uint32_t GetCollisionAttribute() = 0;
	// 
	virtual void SetCollisionAttribute(uint32_t collisionAttribute) = 0;
	// 
	virtual uint32_t GetCollisionMask() = 0;
	// 
	virtual void SetCollisionMask(uint32_t collisionMask) = 0;

private:

	//
	uint32_t collisionAttribute_ = 0xffffffff;
	// 
	uint32_t collisionMask_ = 0xffffffff;
	
	// 
	float m_radius = 1.0f;


};