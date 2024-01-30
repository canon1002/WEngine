#include "Player.h"
#include "../3d/ModelManager.h"

Player::Player() {

}

Player::~Player() {
}

void Player::Init() {

	// モデルをセットする
	ModelManager::GetInstance()->LoadModel("emptyAxis.obj");

	worldTransform_.scale = { 1.0f,1.0f,1.0f };
	worldTransform_.rotate = { 0.0f,0.0f,0.0f };
	worldTransform_.translate = { 0.0f,0.0f,5.0f };
	worldTransform_.worldM = MakeAffineMatrix(worldTransform_.scale,
		worldTransform_.rotate, worldTransform_.translate);

	worldTransformRail_ = worldTransform_;

	object_ = std::make_unique<Object3d>();
	object_->Init();
	object_->SetModel("emptyAxis.obj");
	object_->SetWorldTransform(worldTransform_);

	rad_ = { 12,12,12 };
	vel_ = { 0.1f,0.1f,0.1f };
	speed_ = { 4,4,4 };
	color_ = { 1.0f,1.0f,1.0f,1.0f };
}

void Player::Update() {


	ImGui::Begin("Player");
	ImGui::SliderAngle("RotateX", &worldTransform_.rotate.x);
	ImGui::SliderAngle("RotateY", &worldTransform_.rotate.y);
	ImGui::SliderAngle("RotateZ", &worldTransform_.rotate.z);
	ImGui::DragFloat3("Rotate", &worldTransform_.rotate.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("Transform", &worldTransform_.translate.x, 0.1f, -100.0f, 100.0f);
	ImGui::End();


	// 行列を更新する
	worldTransform_.worldM = MakeAffineMatrix(worldTransform_.scale,
		worldTransform_.rotate, worldTransform_.translate);

	object_->SetWorldTransform(worldTransform_);

	if (camera_ != nullptr) {
	
		worldTransformRail_.scale = worldTransform_.scale;
		worldTransformRail_.rotate = worldTransform_.rotate;
		worldTransformRail_.translate = worldTransform_.translate;
		worldTransformRail_.worldM = MakeAffineMatrix(worldTransformRail_.scale, worldTransformRail_.rotate, worldTransformRail_.translate);
		worldTransformRail_.worldM = Multiply(camera_->GetWorldMatrix(), worldTransform_.worldM);
		object_->SetWorldTransform(worldTransformRail_);
	}
	
	object_->Update();
	
}

void Player::Draw() {
	object_->Draw();
}

void Player::MoveUp() {
	this->worldTransform_.translate.y += this->vel_.y;
	
}

void Player::MoveUpLeft()
{
	this->worldTransform_.translate.x -= this->vel_.x;
	this->worldTransform_.translate.y += this->vel_.y;
	
}

void Player::MoveUpRight()
{
	this->worldTransform_.translate.x += this->vel_.x;
	this->worldTransform_.translate.y += this->vel_.y;

}

void Player::MoveDown() {
	this->worldTransform_.translate.y -= this->vel_.y;
	
}

void Player::MoveDownLeft()
{
	this->worldTransform_.translate.x -= this->vel_.x;
	this->worldTransform_.translate.y -= this->vel_.y;
	
}

void Player::MoveDownRight()
{
	this->worldTransform_.translate.x += this->vel_.x;
	this->worldTransform_.translate.y -= this->vel_.y;
	
}

void Player::MoveLeft() {
	this->worldTransform_.translate.x -= this->vel_.x;
	
}

void Player::MoveRight() {
	this->worldTransform_.translate.x += this->vel_.x;
	
}