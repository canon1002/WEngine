#pragma once
#include "IObject.h"
#include <stdint.h>
#include "Engine/Object/WorldTransform/WorldTransform.h"
#include "Engine/Math/Math.h"
#include "Engine/Object/Texture/Color.h"
#include "Engine/Object/Object3d.h"
#include "Engine/Collision/Collider.h"

class Player;

class PlayerBullet :
    public Collider
{
public:
    void Init(Vec3 pos,Vec3 vel);
    void Init(Player* player);
    void Update();
    void Draw();
    bool GetIsActive()const { return isActive_; }
    void SetCamera(CameraCommon* camera) { object_->SetCamera(camera);
    camera_ = camera;
    }
    WorldTransform GetWorld()const { return worldTransform_; }
    Vec3 GetRad()const { return rad_; }

public:

    // 純粋仮想関数
    void OnCollision()override {
        isActive_ = false; 
    }
    void OnCollision(Collider* collider)override {
        collider; 
        isActive_ = false;
    }

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


private:

    // 参照用
    CameraCommon* camera_ = nullptr;
    Player* player_ = nullptr;

    std::unique_ptr<Object3d> object_;
    WorldTransform worldTransform_;

    Vec3 rad_;
    Vec3 direction;
    Vec3 vel_;
    Vec3 speed_;
    Color color_;
    bool isActive_;

    //
    uint32_t collisionAttribute_ = 0xffffffff;
    // 
    uint32_t collisionMask_ = 0xffffffff;

};

