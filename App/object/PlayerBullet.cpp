#include "PlayerBullet.h"
#include "../3d/ModelManager.h"

void PlayerBullet::Init(Vec3 startPos) {
	worldTransform_.scale = { 1.0f,1.0f,1.0f };
	worldTransform_.rotate = { 0.0f,0.0f,0.0f };
	worldTransform_.translate = startPos;
	worldTransform_.worldM = MakeAffineMatrix(worldTransform_.scale,
		worldTransform_.rotate, worldTransform_.translate);

	rad_ = { 1,1,1 };
	vel_ = { 0.00f,0.00f,0.05f };
	isActive_ = true;

	object_ = std::make_unique<Object3d>();
	object_->Init();
	object_->SetModel("emptyAxis.obj");

}

void PlayerBullet::Init() {
	
	worldTransform_.scale = { 1.0f,1.0f,1.0f };
	worldTransform_.rotate = { 0.0f,0.0f,0.0f };
	worldTransform_.translate = { 0.0f,0.0f,0.0f };
	worldTransform_.worldM = MakeAffineMatrix(worldTransform_.scale,
		worldTransform_.rotate, worldTransform_.translate);
	
	object_ = std::make_unique<Object3d>();
	object_->Init();
	object_->SetModel("emptyAxis.obj");
	object_->SetWorldTransform(worldTransform_);

	rad_ = { 1,1,1 };
	vel_ = { 0.00f,0.00f,0.05f };
	isActive_ = true;

	

}

void PlayerBullet::Update() {
	// 早期リターン
	if (!isActive_) { return; }

	if (isActive_) {
		worldTransform_.translate.z += vel_.z;
		if (worldTransform_.translate.z > 50.0f) {
			isActive_ = false;
		}
	}

	worldTransform_.worldM = MakeAffineMatrix(worldTransform_.scale,
		worldTransform_.rotate, worldTransform_.translate);
	object_->SetWorldTransform(worldTransform_);
	object_->Update();
}

void PlayerBullet::Draw() {
	if (!isActive_) { return; }
	object_->Draw();
}