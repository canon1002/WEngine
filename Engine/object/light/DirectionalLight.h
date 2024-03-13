#pragma once
#include "Engine/Math/Math.h"
#include "Engine/Object/Texture/Color.h"

struct DirectionalLight
{
	// ライトの色
	Color color;
	// ライトの向き
	Vec3 direction;
	// 輝度
	float intensity;	
};

