#include "RayCollider.h"
#include "PlaneCollider.h"
#include "AABBCollider.h"
#include "SphereCollider.h"


RayCollider::RayCollider(Vector3 origin, Vector3 diff){
	mOrigin = origin;
	mDiff = diff;
}

void RayCollider::Init()
{
}

void RayCollider::Update()
{
}

void RayCollider::Draw(Camera camera)
{
	camera;
}

bool RayCollider::IsCollision(Collider* c){
	return c->IsCollision(this);
}

bool RayCollider::IsCollision(AABBCollider* c)
{
	Plane xMin;
	Plane xMAx;

	Plane yMin;
	Plane yMax;

	Plane zMin;
	Plane zMax;

	xMin.normal = { 1, 0, 0 };
	xMin.distance = c->GetMin().x;
	xMAx.normal = { 1, 0, 0 };
	xMAx.distance = c->GetMax().x;

	yMin.normal = { 0, 1, 0 };
	yMin.distance = c->GetMin().y;
	yMax.normal = { 0, 1, 0 };
	yMax.distance = c->GetMax().y;

	zMin.normal = { 0, 0, 1 };
	zMin.distance = c->GetMin().z;
	zMax.normal = { 0, 0, 1 };
	zMax.distance = c->GetMax().z;

	float dot = Dot(xMin.normal, mDiff);
	float tMin = (xMin.distance - Dot(mOrigin, xMin.normal)) / dot;
	dot = Dot(xMAx.normal, mDiff);
	float tMax = (xMAx.distance - Dot(mOrigin, xMAx.normal)) / dot;

	float tNearX = (std::min)(tMin, tMax);
	float tFarX = (std::max)(tMin, tMax);


	dot = Dot(yMin.normal, mDiff);
	tMin = (yMin.distance - Dot(mOrigin, yMin.normal)) / dot;
	dot = Dot(yMax.normal, mDiff);
	tMax = (yMax.distance - Dot(mOrigin, yMax.normal)) / dot;

	float tNearY = (std::min)(tMin, tMax);
	float tFarY = (std::max)(tMin, tMax);

	dot = Dot(zMin.normal, mDiff);
	tMin = (zMin.distance - Dot(mOrigin, zMin.normal)) / dot;
	dot = Dot(zMax.normal, mDiff);
	tMax = (zMax.distance - Dot(mOrigin, zMax.normal)) / dot;

	float tNearZ = (std::min)(tMin, tMax);
	float tFarZ = (std::max)(tMin, tMax);


	float tNear = (std::max)((std::max)(tNearX, tNearY), tNearZ);
	float tFar = (std::min)((std::min)(tFarX, tFarY), tFarZ);

	if (tNear <= tFar) {
		if ((tNear < 1.0f) && (0.0f < tFar)) {
			return true;
		}
	}

	return false;
}

bool RayCollider::IsCollision(SphereCollider* c)
{
	Vector3 m = mOrigin - c->GetWorldPos();
	Vector3 direction = Normalize(mDiff);
	float distance = Dot(m, m) - c->GetRadius() * c->GetRadius();

	if (distance <= 0.0f) {
		float b = Dot(m, direction);
		if (b > 0.0f) { return false; }
		float disc = b * b - distance;
		if (disc < 0.0f) { return false; }
		return true;
	}

	return false;
}

bool RayCollider::IsCollision(RayCollider* c)
{
	c;
	return false;
}

bool RayCollider::IsCollision(PlaneCollider* c)
{
	// 垂直であるかの判定を行う
	float dot = Dot(c->GetNormal(), mDiff);

	// 0除算になるためb・n＝0の場合は計算不能でtが存在しないため、衝突しない
	if (dot == 0.0f) {
		return false;
	}

	// 距離を求める
	float t = (c->GetDistance() - Dot(mOrigin, c->GetNormal())) / dot;

	// 半径の合計よりも短ければ衝突
	if (0.0f < t && t < 2.0f){
		return true;
	}

	return false;
}


Vector3 RayCollider::GetOrigin() const {
	return mOrigin;
}

Vector3 RayCollider::GetDiff() const {
	return mDiff;
}


void RayCollider::CreateTransformation()
{
}
