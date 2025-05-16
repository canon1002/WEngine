#include "Collider.h"

// 前方宣言
class AABBCollider;
class SphereCollider;
class PlaneCollider;

class RayCollider :
    public Collider
{
public: // -- 公開 メンバ関数 -- //

    // コンストラクタ
    RayCollider(Vector3 origin,Vector3 diff);


    void Init()override;
    void Update()override;
    void Draw()override;

    // 衝突判定関数
    virtual bool IsCollision(Collider* c)override;
    virtual bool IsCollision(AABBCollider* c);
    virtual bool IsCollision(SphereCollider* c);
    virtual bool IsCollision(RayCollider* c);
    virtual bool IsCollision(PlaneCollider* c);

    // 衝突時
    virtual void OnCollision()override {};
    virtual void OnCollision(Collider* collider) override { collider; }

    // ワールド座標
    virtual Vector3 GetWorldPos()const override { return mWorldTransform->GetWorldPosition(); }

    Vector3 GetOrigin()const;
    Vector3 GetDiff()const;

private: // -- 非公開 メンバ関数 -- //

    void CreateTransformation()override;

protected:

    Vector3 mOrigin;
    Vector3 mDiff;

};

