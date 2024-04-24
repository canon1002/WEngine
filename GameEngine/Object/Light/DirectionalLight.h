#pragma once
#include "GameEngine/Math/Math.h"
#include "GameEngine/Resource/Material/Color.h"

struct DirectionalLight
{
	// ライトの色
	Color color;
	// ライトの向き
	Vector3 direction;
	// 輝度
	float intensity;	
};

