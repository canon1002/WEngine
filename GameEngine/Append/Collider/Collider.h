#pragma once
#include "GameEngine/Math/Math.h"
#include "CollisionConfig.h"
#include <list>

/// <summary>
/// AABB (軸平行境界ボックス)
/// </summary>
struct AABB {
	Vector3 min;
	Vector3 max;
};

/// <summary>
/// OBB(有向境界ボックス)
/// </summary>
struct OBB {
	Vector3 center; // 中心点
	Vector3 orientations[3]; // 座標軸。正規化・直交必須
	Vector3 size; // 座標方向の長さの半分。中心から面までの距離
};

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
	inline void SetRadius(float radius) { m_radius = radius; }
	inline float GetRadius()const  { return m_radius; }

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