#pragma once
#include "Collider.h"

class CapsuelCollider :
    public Collider
{
  
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="worldtransform"> ワールド座標のポインタ </param>
    /// <param name="radius"> コライダーの半径 </param>
    CapsuelCollider(std::unique_ptr<WorldTransform> worldtransform, float radius);

    /// <summary>
    /// 仮想デストラクタ
    /// </summary>
    virtual ~CapsuelCollider() {};

    /// <summary>
    /// 初期化
    /// </summary>
    void Init()override;

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update()override;

    /// <summary>
    /// 描画処理(デバッグ用)
    /// </summary>
    void Draw()override;

    //
    // -- 衝突判定関数 -- //
    //

    // マネージャーからの呼び出し用
    virtual bool IsCollision(Collider* c)override;
    // AABBとの衝突判定
    virtual bool IsCollision(AABBCollider* c);
    // 球体との衝突判定
    virtual bool IsCollision(SphereCollider* c);
    // OBBとの衝突判定
    virtual bool IsCollision(OBBCollider* c);
	// カプセルとの衝突判定
    virtual bool IsCollision(CapsuelCollider* c);

    // 衝突時の処理
    virtual void OnCollision()override {};
    virtual void OnCollision(Collider* collider) override {
        collider;
        if (mOnCollisionCount == 0) {
            mIsOnCollision = true;
            mOnCollisionCount = 60;
        }
    }

    // ワールド座標の取得
    Vector3 GetWorldPos()const override { return mWorldTransform->GetWorldPosition(); }

    // 半径を設定
    float GetRadius()const { return mRadius; }



private: // -- 非公開 メンバ関数 -- //

    /// <summary>
    /// デバッグ用の座標リソースの生成
    /// </summary>
    void CreateTransformation()override;


protected:

    // カプセルの始点
	Vector3 mOrigin;
    // カプセルの終点への差分ベクトル
    Vector3 mDiff;
    // カプセルの半径
    float mRadius;

};

