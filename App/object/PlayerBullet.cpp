#include "PlayerBullet.h"

void PlayerBullet::Init(Vec2 startPos) {
	this->pos_ = startPos;
	rad_ = { 16,16 };
	vel_ = { 16, 16 };
	isActive_ = true;
}

void PlayerBullet::Init() {
	this->pos_ = { -100 ,-100 };
	rad_ = { 16,16 };
	vel_ = { 16, 16 };
	isActive_ = true;
}

void PlayerBullet::Update() {
	// 早期リターン
	if (!isActive_) { return; }

	if (isActive_) {
		pos_.y -= vel_.y;
		if (pos_.y < -50) {
			isActive_ = false;
		}
	}
}

void PlayerBullet::Draw() {
	if (!isActive_) { return; }
	
}