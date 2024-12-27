#include "Collider.h"

// 前方宣言
class AABBCollider;
class SphereCollider;
class RayCollider;

class PlaneCollider :
    public Collider
{
public: // -- 公開 メンバ関数 -- //

    // コンストラクタ
    PlaneCollider(Vector3 normal, float distance);


    void Init()override;
    void Update()override;
    void Draw()override;

    // 衝突判定関数
    virtual bool IsCollision(Collider* c)override;
    virtual bool IsCollision(AABBCollider* c);
    virtual bool IsCollision(SphereCollider* c);
    virtual bool IsCollision(RayCollider* c);

    // 衝突時
    virtual void OnCollision()override {};
    virtual void OnCollision(Collider* collider) override { collider; }

    // ワールド座標
    virtual Vector3 GetWorldPos()const override { return mWorldTransform->GetWorldPosition(); }

    Vector3 GetNormal()const;
    float GetDistance()const;

private: // -- 非公開 メンバ関数 -- //

    void CreateTransformation()override;

protected:

    // 法線(単位ベクトル)
    Vector3 mNormal;
    // 距離
    float mDistance;

};

