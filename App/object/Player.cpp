#include "Player.h"
#include "../3d/ModelManager.h"

Player::Player() {

}

Player::~Player() {
}

void Player::Init() {

	// モデルをセットする
	ModelManager::GetInstance()->LoadModel("box.obj");

	worldTransform_.scale = { 1.0f,1.0f,1.0f };
	worldTransform_.rotate = { 0.0f,0.0f,0.0f };
	worldTransform_.translate = { 0.0f,0.0f,15.0f };
	worldTransform_.worldM = MakeAffineMatrix(worldTransform_.scale,
		worldTransform_.rotate, worldTransform_.translate);

	worldTransformRail_ = worldTransform_;

	rad_ = { 1,1,1 };
	vel_ = { 0.1f,0.1f,0.1f };
	speed_ = { 4,4,4 };
	color_ = { 1.0f,1.0f,1.0f,1.0f };

	object_ = std::make_unique<Object3d>();
	object_->Init();
	object_->SetModel("box.obj");
	object_->SetWorldTransform(worldTransform_);

	collisionAttribute_ = kCollisionAttributePlayer;
	collisionMask_ = kCollisionAttributeEnemy + kCollisionAttributeEnemyBullet;

	// レティクル
	reticle = std::make_unique<Reticle>();
	reticle->Initialze();

}

void Player::Update() {

	// 行列を更新する

#ifdef _DEBUG

	ImGui::Begin("Player");
	ImGui::SliderAngle("RotateX", &worldTransform_.rotate.x);
	ImGui::SliderAngle("RotateY", &worldTransform_.rotate.y);
	ImGui::SliderAngle("RotateZ", &worldTransform_.rotate.z);
	ImGui::DragFloat3("Rotate", &worldTransform_.rotate.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("Transform", &worldTransform_.translate.x, 0.1f, -100.0f, 100.0f);
	ImGui::End();


#endif // _DEBUG


	if (camera_ != nullptr) {
		worldTransform_.worldM = MakeAffineMatrix(
			worldTransform_.scale,
			worldTransform_.rotate, {
			worldTransform_.translate.x + camera_->GetTransform().translate.x,
			worldTransform_.translate.y + camera_->GetTransform().translate.y,
			worldTransform_.translate.z + camera_->GetTransform().translate.z
			});
	}
	else {
		worldTransform_.worldM = MakeAffineMatrix(
			worldTransform_.scale,
			worldTransform_.rotate,
			worldTransform_.translate
		);
	}

	object_->SetWorldTransform(worldTransform_);

	object_->Update();
	reticle->Update();
}

void Player::Draw() {
	object_->Draw();
	reticle->DrawModel();
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

Vec3 Player::GetReticleAxis()
{

	// 弾の速度を設定
	const float kBulletSpeed = 0.5f;

	// 差分を計算
	Vec3 SubPos{ 0, 0, 0 };
	SubPos = Subtract(reticle->GetWorld().translate, worldTransform_.translate);
	// 長さを調整
	SubPos = Nomalize(SubPos);

	Vec3 velocity{ 0, 0, 0 };
	velocity.x = SubPos.x * kBulletSpeed;
	velocity.y = SubPos.y * kBulletSpeed;
	velocity.z = SubPos.z * kBulletSpeed;

	// 射撃クールタイムを規定値にあわせる
	//m_shotCoolTime = kShotCoolTimeMax;

	return velocity;
}
