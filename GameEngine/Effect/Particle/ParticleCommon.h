#pragma once
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include <random>

class CameraCommon;

// パーティクル1つの要素
struct Particle {
	WorldTransform worldTransform;
	Vector3 vel;
	Color color;
	float lifeTime;
	float currentTime;
};

// エミッター(パーティクル発生装置)
struct Emitter {
	Vector3 pos;// エミッターの座標
	uint32_t count;// 発生数
	std::uniform_real_distribution<float> posXDistr; // 発生X座標の分布
	std::uniform_real_distribution<float> posYDistr; // 発生Y座標の分布
	std::uniform_real_distribution<float> posZDistr; // 発生Z座標の分布
	std::uniform_real_distribution<float> scaleDistr; // 拡大率の分布
	std::uniform_real_distribution<float> velXDistr; // X軸移動量の分布
	std::uniform_real_distribution<float> velYDistr; // Y軸移動量の分布
	std::uniform_real_distribution<float> velZDistr; // Z軸移動量の分布
	std::uniform_real_distribution<float> colorDistr; // 発生時の色の分布
	std::uniform_real_distribution<float> timeDistr; // 生存時間の分布

};
