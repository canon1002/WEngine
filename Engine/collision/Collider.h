#pragma once
#include "Vector3.h"
#include "CollisionConfig.h"
#include <list>

/// <summary>
///	・ﾕ・ﾋ・ｻ・・・I・u・W・F・N・g
/// <summary>
class Collider
{
public:

	// ・ﾕ・ﾋ・・・ﾌ・・・・
	virtual void OnCollision() = 0;
	virtual void OnCollision(Collider* collider) = 0;
	
	// ・ﾀ・W
	virtual Vector3 GetWorldPos() = 0;
	
	// ・ｼ・a
	void SetRadius(float radius) { m_radius = radius; }
	float GetRadius() { return m_radius; }

	// ・ﾕ・ﾋ・ｮ・ｫ(・ｩ・ｪ)・・・・・ｾ
	virtual uint32_t GetCollisionAttribute() = 0;
	// ・ﾕ・ﾋ・ｮ・ｫ(・ｩ・ｪ)・・・ﾝ・・
	virtual void SetCollisionAttribute(uint32_t collisionAttribute) = 0;
	// ・ﾕ・ﾋ・}・X・N(・・・・)・・・・・ｾ
	virtual uint32_t GetCollisionMask() = 0;
	// ・ﾕ・ﾋ・}・X・N(・・・・)・・・ﾝ・・
	virtual void SetCollisionMask(uint32_t collisionMask) = 0;

private:

	//
	uint32_t m_collisionAttribute = 0xffffffff;
	// 
	uint32_t m_collisionMask = 0xffffffff;
	
	// ・ｼ・a
	float m_radius = 1.0f;


};