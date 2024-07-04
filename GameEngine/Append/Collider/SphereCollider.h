#include "Collider.h"

class SphereCollider :
    public Collider
{
public:

    SphereCollider(WorldTransform* worldtransform, float radius);

    // 衝突時
    virtual void OnCollision()override {};
    virtual void OnCollision(Collider* collider) override { collider; }

    // ワールド座標
    Vector3 GetWorldPos()const override { return pWorldTransform->GetWorldPosition(); }

    float GetRadius()const { return mRadius; }

protected:

    // ワールド座標系のポインタ
    WorldTransform* pWorldTransform;

    // 球体の半径
    float mRadius;

};
