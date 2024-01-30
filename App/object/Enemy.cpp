#include "Enemy.h"
#include "../3d/ModelManager.h"

Enemy::Enemy() {}

Enemy::~Enemy() {}


void Enemy::Init() {

	// モデルをセットする
	ModelManager::GetInstance()->LoadModel("box.obj");

	worldTransform_.scale = { 1.0f,1.0f,1.0f };
	worldTransform_.rotate = { 0.0f,0.0f,0.0f };
	worldTransform_.translate = { 0.0f,0.0f,40.0f };
	worldTransform_.worldM = MakeAffineMatrix(worldTransform_.scale,
		worldTransform_.rotate, worldTransform_.translate);

	worldTransformRail_ = worldTransform_;

	object_ = std::make_unique<Object3d>();
	object_->Init();
	object_->SetModel("box.obj");
	object_->SetWorldTransform(worldTransform_);

	rad_ = { 1.0f,1.0f,1.0f };
	vel_ = { 0.0f,0.0f,0.0f };
	isActive_ = true;


	collisionAttribute_ = kCollisionAttributeEnemy;
	collisionMask_ = kCollisionAttributePlayer + kCollisionAttributePlayerBullet;
}

void Enemy::Update() {

	// 早期リターン
	if (!isActive_) { return; }

	/*if (isActive_) {
		worldTransform_.translate.z += vel_.z;
		if (worldTransform_.translate.z > 50.0f) {
			isActive_ = false;
		}
	}*/

	worldTransform_.worldM = MakeAffineMatrix(worldTransform_.scale,
		worldTransform_.rotate, worldTransform_.translate);
	object_->SetWorldTransform(worldTransform_);
	object_->Update();

}

void Enemy::Draw() {

	if (!isActive_) { return; }
	object_->Draw();

}