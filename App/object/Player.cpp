#include "Player.h"

Player::Player() {

}

Player::~Player() {

}

void Player::Init() {

	model_ = std::make_unique<Model>();
	model_->Initialize();

	pos_ = { 0,0 };
	rad_ = { 12,12 };
	vel_ = { 4,4,4 };
	speed_ = { 4,4,4 };
	color_ = { 1.0f,1.0f,1.0f,1.0f };
}

void Player::Update() {
	model_->Update();
}

void Player::Draw() {
	model_->Draw();
}

void Player::MoveUp() {
	this->pos_.y -= this->vel_.y;
}

void Player::MoveDown() {
	this->pos_.y += this->vel_.y;
}

void Player::MoveLeft() {
	this->pos_.x -= this->vel_.x;
}

void Player::MoveRight() {
	this->pos_.x += this->vel_.x;
}