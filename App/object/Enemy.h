#pragma once
#include "IObject.h"
#include "PlayerBullet.h"
#include <list>

class Enemy :
    public IObject
{
public:
    Enemy();
    ~Enemy();
    void Init()override;
    void Init(PlayerBullet* playerBullet);
    void Update()override;
    void Draw()override;
    bool GetIsActive()const { return isActive_; }
    Vec3 GetPos()const { return pos_; }
    Vec3 GetRad()const { return rad_; }
public:

    Vec3 pos_;
    Vec3 rad_;
    Vec3 vel_;
    bool isActive_;

};

