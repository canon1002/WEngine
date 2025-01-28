#include "CapsuelCollider.h"

void CapsuelCollider::Init()
{
}

void CapsuelCollider::Update()
{
}

void CapsuelCollider::Draw()
{
}

bool CapsuelCollider::IsCollision(Collider* c)
{
	c;
	return false;
}

bool CapsuelCollider::IsCollision(AABBCollider* c)
{
	c;
	return false;
}

bool CapsuelCollider::IsCollision(SphereCollider* c)
{
	c;
	return false;
}

bool CapsuelCollider::IsCollision(OBBCollider* c)
{
	c;
	return false;
}

bool CapsuelCollider::IsCollision(CapsuelCollider* c)
{

	// 2線が平行であるかを判定する
	if (Dot(this->mDiff, c->mDiff) == 0.0f) {
		// 平行である場合


	}


	return false;
}

void CapsuelCollider::CreateTransformation()
{
}
