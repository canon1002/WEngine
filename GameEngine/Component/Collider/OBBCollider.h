#pragma once
#include "Collider.h"

// 前方宣言
class SphereCollider;
class AABBCollider;

class OBBCollider :
    public Collider
{
public: // -- 公開 メンバ関数 -- //

    // コンストラクタ
    OBBCollider(WorldTransform* worldtransform, Vector3 size);


    void Init()override;
    void Update()override;
    void Draw()override;

    // 衝突判定関数
    virtual bool IsCollision(Collider* c)override;
    virtual bool IsCollision(AABBCollider* c);
    virtual bool IsCollision(SphereCollider* c);
    virtual bool IsCollision(OBBCollider* c);

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

    // 移動量取得
    Vector3 GetVelocity();

    // 衝突フラグ取得
    virtual bool GetOnCollisionFlag()const override { return mIsOnCollision; }

    bool OverlapOnAxis(OBBCollider* a, OBBCollider* b, const Vector3& axis);
    bool OverlapOnAxis(OBBCollider* obb, AABBCollider* aabb, const Vector3& axis);

	void ProjectOBB(OBBCollider* obb, const Vector3& axis, float& min, float& max);
    void ProjectAABB(AABBCollider* aabb, const Vector3& axis, float& min, float& max);

    // 頂点を取得
    std::vector<Vector3> GetVertices() const {
        std::vector<Vector3> vertices(8);
        // OBBの中心と半径を取得
        Vector3 center = mWorldTransform->GetWorldPosition();
        Vector3 halfSize = mSize * 0.5f;

        // 各頂点を計算
        vertices[0] = center + Vector3(-halfSize.x, -halfSize.y, -halfSize.z);
        vertices[1] = center + Vector3(halfSize.x, -halfSize.y, -halfSize.z);
        vertices[2] = center + Vector3(-halfSize.x, halfSize.y, -halfSize.z);
        vertices[3] = center + Vector3(halfSize.x, halfSize.y, -halfSize.z);
        vertices[4] = center + Vector3(-halfSize.x, -halfSize.y, halfSize.z);
        vertices[5] = center + Vector3(halfSize.x, -halfSize.y, halfSize.z);
        vertices[6] = center + Vector3(-halfSize.x, halfSize.y, halfSize.z);
        vertices[7] = center + Vector3(halfSize.x, halfSize.y, halfSize.z);

        return vertices;
    }

	// 指定軸番号の方向ベクトル取得
	Vector3 GetDirect(int num) { return mOrientations[num]; }
	// 指定軸方向の長さ取得
	Vector3 GetLen()const { return mSize; }
    // サイズ取得
	Vector3 GetSize()const { return mSize; }

private: // -- 非公開 メンバ関数 -- //

    void CreateTransformation()override;

protected:

    // 座標軸(正規化・直交必須)
	Vector3 mOrientations[3];
    // 座標軸方向の長さの半分
    Vector3 mSize;

    // 前フレームの座標
    Vector3 mPrePosition;

};

