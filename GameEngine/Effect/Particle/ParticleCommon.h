#pragma once
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Append/Transform/WorldTransform.h"

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
	std::unique_ptr<WorldTransform> worldtransform;// エミッターの座標
	uint32_t count;// 発生数
	float frequency;// 発生頻度
	float frequencyTime;// 頻度用時刻
};
