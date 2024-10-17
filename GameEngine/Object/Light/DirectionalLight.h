#pragma once
#include "Math/Math.h"
#include "Resource/Material/Color.h"

struct DirectionalLight
{
	// ライトの色
	Color color;
	// ライトの向き
	Vector3 direction;
	// 輝度
	float intensity;	
};

