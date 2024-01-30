#include "Skydome.h"
#include "../3d/ModelManager.h"

void Skydome::Initialize()
{
	// モデルをセットする
	ModelManager::GetInstance()->LoadModel("skydome.obj");

	worldTransform_.scale = { 64.0f,64.0f,64.0f };
	worldTransform_.rotate = { 0.0f,0.0f,0.0f };
	worldTransform_.translate = { 0.0f,0.0f,0.0f };
	worldTransform_.worldM = MakeAffineMatrix(worldTransform_.scale,
		worldTransform_.rotate, worldTransform_.translate);

	object_ = std::make_unique<Object3d>();
	object_->Init();
	object_->SetScale(worldTransform_.scale);
	object_->SetModel("skydome.obj");
	object_->SetWorldTransform(worldTransform_);
}

void Skydome::Update()
{
	worldTransform_.worldM = MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);

	object_->Update();
}

void Skydome::Draw()
{
	object_->Draw();
}
