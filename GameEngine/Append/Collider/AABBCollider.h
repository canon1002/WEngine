#pragma once
#include "Collider.h"

// 前方宣言
class SphereCollider;

class AABBCollider :
    public Collider
{
public: // -- 公開 メンバ関数 -- //

    // コンストラクタ
    AABBCollider(WorldTransform* worldtransform,Vector3 radius);


    void Init()override;
    void Update()override;
    void Draw()override;

    // 衝突判定関数
    virtual bool IsCollision(Collider* c)override;
    virtual bool IsCollision(AABBCollider* c);
    virtual bool IsCollision(SphereCollider* c);

    // 衝突時
    virtual void OnCollision()override {};
    virtual void OnCollision(Collider* collider) override { collider; }

    // ワールド座標
    virtual Vector3 GetWorldPos()const override { return pWorldTransform->GetWorldPosition(); }

    Vector3 GetMin()const;
    Vector3 GetMax()const;

private: // -- 非公開 メンバ関数 -- //

    void CreateTransformation()override;

protected:

    // AABBの各ベクトルごとの半径
    Vector3 mRadius;

};
