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
    ~AABBCollider() = default;

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
    virtual Vector3 GetWorldPos()const override { return mWorldTransform->GetWorldPosition(); }

    Vector3 GetMin()const;
    Vector3 GetMax()const;

    // 移動量取得
    Vector3 GetVelocity();

    // 衝突フラグ取得
    virtual bool GetOnCollisionFlag()const override{ return mIsOnCollision; }

	// 頂点を取得
    std::vector<Vector3> GetVertices() const {
        std::vector<Vector3> vertices(8);
        // AABBの最小点と最大点を取得
        Vector3 min = GetMin();
        Vector3 max = GetMax();

        // 各頂点を計算
        vertices[0] = Vector3(min.x, min.y, min.z);
        vertices[1] = Vector3(max.x, min.y, min.z);
        vertices[2] = Vector3(min.x, max.y, min.z);
        vertices[3] = Vector3(max.x, max.y, min.z);
        vertices[4] = Vector3(min.x, min.y, max.z);
        vertices[5] = Vector3(max.x, min.y, max.z);
        vertices[6] = Vector3(min.x, max.y, max.z);
        vertices[7] = Vector3(max.x, max.y, max.z);

        return vertices;
    }

private: // -- 非公開 メンバ関数 -- //

    void CreateTransformation()override;

protected:

    // AABBの各ベクトルごとの半径
    Vector3 mRadius;
    // 前フレームの座標
    Vector3 mPrePosition;

};

