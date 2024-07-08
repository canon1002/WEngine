#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "RayCollider.h"
#include "AABBCollider.h"


SphereCollider::SphereCollider(WorldTransform* worldtransform, float radius){
	// ワールド座標のポインタを代入
	pWorldTransform = worldtransform;
	// 半径を代入
	mRadius = radius;
}
