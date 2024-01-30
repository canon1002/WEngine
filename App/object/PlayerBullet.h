#pragma once
#include "IObject.h"
#include <stdint.h>
#include "../worldTransform/WorldTransform.h"
#include "../math/Math.h"
#include "../color/Color.h"
#include "../3d/Object3d.h"

class PlayerBullet :
    public IObject
{
public:
    void Init(Vec3 startPos);
    void Init() override;
    void Update() override;
    void Draw() override;
    bool GetIsActive()const { return isActive_; }
    void SetCamera(CameraCommon* camera) { object_->SetCamera(camera); }
    WorldTransform GetWorld()const { return worldTransform_; }
    Vec3 GetRad()const { return rad_; }
private:

    std::unique_ptr<Object3d> object_;
    WorldTransform worldTransform_;

    Vec3 rad_;
    Vec3 vel_;
    Vec3 speed_;
    Color color_;
    bool isActive_;
};

