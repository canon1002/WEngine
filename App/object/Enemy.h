#pragma once
#include "IObject.h"
#include "PlayerBullet.h"
#include <list>
#include "../collision/Collider.h"

class Enemy :
    public Collider
{
public:
    Enemy();
    ~Enemy();
    void Init();
    void Update();
    void Draw();
    bool GetIsActive()const { return isActive_; }
    Vec3 GetRad()const { return rad_; }

    WorldTransform GetWorld() { return worldTransform_; }
    Vec3 GetWorldRailT() {
        return { worldTransformRail_.worldM.m[3][0],
            worldTransformRail_.worldM.m[3][1], worldTransformRail_.worldM.m[3][2], };
    }

    void SetCamera(CameraCommon* camera) {
        object_->SetCamera(camera);
        camera_ = camera;
    }

public:

    // 純粋仮想関数
    void OnCollision()override { isActive_ = false; }
    void OnCollision(Collider* collider)override { isActive_ = false; }

    // ワールド座標
    Vec3 GetWorldPos()override { return worldTransform_.translate; }

    // コライダー
    uint32_t GetCollisionAttribute()override { return collisionAttribute_; }
    // 
    void SetCollisionAttribute(uint32_t collisionAttribute)override { collisionAttribute_ = collisionAttribute; }
    // 
    uint32_t GetCollisionMask()override { return collisionMask_; };
    // 
    void SetCollisionMask(uint32_t collisionMask)override { collisionMask_ = collisionMask; }

public:

    // 参照用
    CameraCommon* camera_ = nullptr;

    std::unique_ptr<Object3d> object_;
    WorldTransform worldTransform_;
    WorldTransform worldTransformRail_;

    Vec3 rad_;
    Vec3 vel_;
    bool isActive_;

    //
    uint32_t collisionAttribute_ = 0xffffffff;
    // 
    uint32_t collisionMask_ = 0xffffffff;

};

