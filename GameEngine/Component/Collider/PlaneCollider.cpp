#include "PlaneCollider.h"
#include "RayCollider.h"
#include "AABBCollider.h"
#include "SphereCollider.h"

PlaneCollider::PlaneCollider(Vector3 normal, float distance){
	mNormal = normal;
	mDistance = distance;
}

void PlaneCollider::Init()
{
}

void PlaneCollider::Update()
{
}

void PlaneCollider::Draw(Camera camera)
{
	camera;
}

bool PlaneCollider::IsCollision(Collider* c){
	return c->IsCollision(this);
}

bool PlaneCollider::IsCollision(AABBCollider* c)
{
	// AABBの中心点を取得
	Vector3 center = c->GetWorldPos();
	// 正の範囲を計算
	Vector3 e = c->GetMax() - center;

	// AABBの半径をL(t) = AABB.center + t * Plane.normal 上に対して射影してできる間隔を計算
	float range = e.x * std::abs(mNormal.x) + e.y * std::abs(mNormal.y) + e.z * std::abs(mNormal.z);

	// 平面からAABBまでの距離を計算
	float distance = Dot(mNormal, center) - mDistance;

	// 距離(distance)が間隔[-r,+r]の範囲内に収まる場合は交差が起こる
	if (std::abs(distance) <= range) {
		return true;
	}

	return false;
}

bool PlaneCollider::IsCollision(SphereCollider* c)
{
	// 絶対値を求める
	float disance = std::fabsf(Dot(mNormal, c->GetWorldPos()) - mDistance);

	// 半径の合計よりも短ければ衝突
	if (disance <= c->GetRadius())
	{
		return true;
	}

	return false;
}

bool PlaneCollider::IsCollision(RayCollider* c)
{
	// 垂直であるかの判定を行う
	float dot = Dot(mNormal, c->GetDiff());

	// 0除算になるためb・n＝0の場合は計算不能でtが存在しないため、衝突しない
	if (dot == 0.0f) {
		return false;
	}

	// 距離を求める
	float distance = (mDistance - Dot(c->GetOrigin(), mNormal)) / dot;

	// 半径の合計よりも短ければ衝突
	if (0.0f < distance && distance < 2.0f){
		return true;
	}

	return false;

}

Vector3 PlaneCollider::GetNormal() const {
	return mNormal;
}

float PlaneCollider::GetDistance() const {
	return mDistance;
}

void PlaneCollider::CreateTransformation()
{
}
