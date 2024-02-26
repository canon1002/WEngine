#include "PlayerBullet.h"
#include "../3d/ModelManager.h"

void PlayerBullet::Init(Vec3 pos, Vec3 vel) {

	ModelManager::GetInstance()->LoadModel("box.obj");

	worldTransform_.scale = { 1.0f,1.0f,1.0f };
	worldTransform_.rotate = { 0.0f,0.0f,0.0f };
	worldTransform_.translate = pos;
	worldTransform_.worldM = MakeAffineMatrix(worldTransform_.scale,
		worldTransform_.rotate, worldTransform_.translate);

	rad_ = { 1,1,1 };
	vel_ = vel;
	isActive_ = true;

	object_ = std::make_unique<Object3d>();
	object_->Init();
	object_->SetModel("box.obj");

	collisionAttribute_ = kCollisionAttributePlayerBullet;
	collisionMask_ = kCollisionAttributeEnemy + kCollisionAttributeEnemyBullet;

}

void PlayerBullet::Init() {
	
	worldTransform_.scale = { 1.0f,1.0f,1.0f };
	worldTransform_.rotate = { 0.0f,0.0f,0.0f };
	worldTransform_.translate = { 0.0f,0.0f,5.0f };
	worldTransform_.worldM = MakeAffineMatrix(worldTransform_.scale,
		worldTransform_.rotate, worldTransform_.translate);
	
	object_ = std::make_unique<Object3d>();
	object_->Init();
	object_->SetModel("emptyAxis.obj");
	object_->SetWorldTransform(worldTransform_);

	rad_ = { 1.0f,1.0f,1.0f };
	vel_ = { 0.00f,0.00f,0.1f };
	isActive_ = true;

	collisionAttribute_ = kCollisionAttributePlayerBullet;
	collisionMask_ = kCollisionAttributeEnemy + kCollisionAttributeEnemyBullet;

}

void PlayerBullet::Update() {
	// 早期リターン
	if (!isActive_) { return; }
#ifdef _DEBUG

	ImGui::Begin("PlayerBullet");
	ImGui::SliderAngle("RotateX", &worldTransform_.rotate.x);
	ImGui::SliderAngle("RotateY", &worldTransform_.rotate.y);
	ImGui::SliderAngle("RotateZ", &worldTransform_.rotate.z);
	ImGui::DragFloat3("Rotate", &worldTransform_.rotate.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("Transform", &worldTransform_.translate.x, 0.1f, -100.0f, 100.0f);
	ImGui::End();

#endif // _DEBUG

	if (isActive_) {
		worldTransform_.translate.x += vel_.x;
		worldTransform_.translate.y += vel_.y;
		worldTransform_.translate.z += vel_.z;
		if (worldTransform_.translate.z > 100.0f) {
			isActive_ = false;
		}
	}

	worldTransform_.worldM = MakeAffineMatrix(
		worldTransform_.scale,
		worldTransform_.rotate, {
		worldTransform_.translate.x + camera_->GetTransform().translate.x,
		worldTransform_.translate.y + camera_->GetTransform().translate.y,
		worldTransform_.translate.z + camera_->GetTransform().translate.z
		});

	object_->SetWorldTransform(worldTransform_);
	object_->Update();
}

void PlayerBullet::Draw() {
	if (!isActive_) { return; }
	object_->Draw();
}