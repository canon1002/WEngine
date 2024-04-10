#include "Enemy.h"
#include "Engine/Object/Model/ModelManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Object/Camera/MainCamera.h"

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
	vel_ = { 0.0f,0.0f,0.01f };
	isActive_ = true;

	// 描画のためのカメラオブジェクトのポインタがなければ取得しておく
	if (camera_ == nullptr) { camera_ = MainCamera::GetInstance(); }

	collisionAttribute_ = kCollisionAttributeEnemy;
	collisionMask_ = kCollisionAttributePlayer + kCollisionAttributePlayerBullet;

	stateManager_ = std::make_unique<EnemyStateManager>();
	stateManager_->Init(this);
}


void Enemy::Init(Vec3 pos) {

	// モデルをセットする
	ModelManager::GetInstance()->LoadModel("box.obj");

	worldTransform_.scale = { 1.0f,1.0f,1.0f };
	worldTransform_.rotate = { 0.0f,0.0f,0.0f };
	worldTransform_.translate = pos;
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
	isBulletShot_ = false;
	
	// 描画のためのカメラオブジェクトのポインタがなければ取得しておく
	if (camera_ == nullptr) { camera_ = MainCamera::GetInstance(); }

	collisionAttribute_ = kCollisionAttributeEnemy;
	collisionMask_ = kCollisionAttributePlayer + kCollisionAttributePlayerBullet;

	stateManager_ = std::make_unique<EnemyStateManager>();
	stateManager_->Init(this);
}

void Enemy::Update() {

	// 早期リターン
	if (!isActive_) { return; }

#ifdef _DEBUG

	ImGui::Begin("Enemy");
	ImGui::SliderAngle("RotateX", &worldTransform_.rotate.x);
	ImGui::SliderAngle("RotateY", &worldTransform_.rotate.y);
	ImGui::SliderAngle("RotateZ", &worldTransform_.rotate.z);
	ImGui::DragFloat3("Rotate", &worldTransform_.rotate.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("Transform", &worldTransform_.translate.x, 0.1f, -100.0f, 100.0f);
	ImGui::End();

#endif // _DEBUG

	// 生存時
	if (isActive_) {
		// 状態に応じた行動を行う
		stateManager_->StateExecution(this);
		Vec3 newRotate = { 0.0f,object_->GetWorldTransform().rotate.y + 0.05f,0.0f };
	
		/*worldTransform_.worldM = MakeAffineMatrix(
			worldTransform_.scale,
			newRotate, {
			worldTransform_.translate.x + camera_->GetTransform().translate.x,
			worldTransform_.translate.y + camera_->GetTransform().translate.y,
			worldTransform_.translate.z + camera_->GetTransform().translate.z
			});*/

		worldTransform_.worldM = MakeAffineMatrix(
			worldTransform_.scale,
			worldTransform_.rotate, {
			worldTransform_.translate.x,
			worldTransform_.translate.y,
			worldTransform_.translate.z
			});

		object_->SetWorldTransform(worldTransform_);
		object_->SetRotate(newRotate);
	}

	object_->UpdateWorldMat();
	object_->Update();

}

void Enemy::Draw() {

	if (!isActive_) { return; }
	object_->Draw();

}