#pragma once
#include "Math.h"
namespace W::Obj{
    class WorldTransform
    {
    public:

        Matrix4x4 GetWorldMatrix() {
            Matrix4x4 result;
            return result;
        }

        // ワールド座標
        Transform transform_;





    };

}