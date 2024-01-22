#include "Enemy.h"

Enemy::Enemy() { this->Init(); }

Enemy::~Enemy() {}


void Enemy::Init() {
	this->pos_ = { 0 ,60,40 };
	rad_ = { 32,32,32 };
	vel_ = { 0,0,6 };
	isActive_ = true;
}

void Enemy::Update() {
	if (!isActive_) { return; }
	// 左右に移動する
	pos_ += vel_;
	if ((pos_.x-rad_.x) <= 100|| (pos_.x + rad_.x) >= 1180) {
		vel_.x *= -1;
	}

}

void Enemy::Draw() {

	if (!isActive_) { return; }
	

}