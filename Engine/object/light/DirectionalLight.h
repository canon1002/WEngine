#pragma once
#include"../../VMQ/Math.h"

struct DirectionalLight
{
	// ライトの色
	Vector4 color;
	// ライトの向き
	Vector3 direction;
	// 輝度
	float intensity;	
};

