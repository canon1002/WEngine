#pragma once
#include "GameEngine/Component/Collider/Collider.h"

// コライダーに要素を追加する
struct GameCollider {
	std::unique_ptr<Collider> collider;// コライダー
	float power = 1.0f; // 威力
	bool isKnockBack = false; // ノックバックが発生する攻撃であるか
};
