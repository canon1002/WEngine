#pragma once
#include "IObject.h"
class PlayerBullet :
    public IObject
{
public:
    void Init(Vec2 startPos);
    void Init() override;
    void Update() override;
    void Draw() override;
    bool GetIsActive()const { return isActive_; }
    Vec2 GetPos()const { return pos_; }
    Vec2 GetRad()const { return rad_; }
private:

    Vec2 pos_;
    Vec2 rad_;
    Vec2 vel_;
    bool isActive_;
};

