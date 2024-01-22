#pragma once
#include"../../math/Math.h"
#include"../color/Color.h"

struct DirectionalLight
{
	// ライトの色
	Color color;
	// ライトの向き
	Vec3 direction;
	// 輝度
	float intensity;	
};

