#pragma once
#include "Collider.h"

class AABBCollider;

class SphereCollider :
    public Collider
{
public:

    SphereCollider(WorldTransform* worldtransform, float radius);

    void Init()override;
    void Update()override;
    void Draw()override;

    // 衝突判定関数
    virtual bool IsCollision(Collider* c)override;
    virtual bool IsCollision(AABBCollider* c);
    virtual bool IsCollision(SphereCollider* c);

    // 衝突時
    virtual void OnCollision()override {};
    virtual void OnCollision(Collider* collider) override {
        collider;
        if (mOnCollisionCount == 0) {
            mIsOnCollision = true;
            mOnCollisionCount = 60;
        }
    }

    // ワールド座標
    Vector3 GetWorldPos()const override { return pWorldTransform->GetWorldPosition(); }
    // 半径の設定
    float GetRadius()const { return mRadius; }

   

private: // -- 非公開 メンバ関数 -- //

    void CreateTransformation()override;


protected:

    // ワールド座標系のポインタ
    //WorldTransform* pWorldTransform;

    // 球体の半径
    float mRadius;
 

};
