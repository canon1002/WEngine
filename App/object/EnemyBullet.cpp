#include "EnemyBullet.h"
#include "Engine/Object/Model/ModelManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "App/object/Enemy.h"
#include "App/object/Player.h"
#include "Engine/Object/Camera/MainCamera.h"

void EnemyBullet::Init(Vec3 pos, Vec3 vel) {

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

	collisionAttribute_ = kCollisionAttributeEnemyBullet;
	collisionMask_ = kCollisionAttributePlayer + kCollisionAttributePlayerBullet;

}

void EnemyBullet::Init(Enemy* enemy, Player* player) {
	enemy_ = enemy;
	player_ = player;

	worldTransform_.scale = { 1.0f,1.0f,1.0f };
	worldTransform_.rotate = { 0.0f,0.0f,0.0f };
	worldTransform_.translate = enemy_->GetWorldPos();
	worldTransform_.worldM = MakeAffineMatrix(worldTransform_.scale,
		worldTransform_.rotate, worldTransform_.translate);

	object_ = std::make_unique<Object3d>();
	object_->Init();
	object_->SetModel("emptyAxis.obj");
	object_->SetWorldTransform(worldTransform_);

	rad_ = { 1.0f,1.0f,1.0f };
	// プレイヤーとレティクルの２点から方向を求める
	direction = Subtract(player_->GetWorldPos(), enemy_->GetWorldPos());
	// 正規化してベクトルの長さを整える
	direction = Normalize(direction);
	// 移動量は方向*移動速度
	vel_.x = direction.x * 0.5f;
	vel_.y = direction.y * 0.5f;
	vel_.z = direction.z * 0.5f;
	//回転
	Vec3 rotate = { 0.0f,0.0f,0.0f };
	const float PI = 3.14f;
	rotate.y = std::atan2f(direction.x, direction.z);
	rotate.y = std::fmodf(rotate.y, 2.0f * PI);
	// 回転量が超過していたり、下限を下回っていたら補正する
	if (rotate.y > PI) { rotate.y -= 2.0f * PI; }
	if (rotate.y < -PI) { rotate.y += 2.0f * PI; }
	worldTransform_.rotate = rotate;

	isActive_ = true;

	collisionAttribute_ = kCollisionAttributeEnemyBullet;
	collisionMask_ = kCollisionAttributeEnemy + kCollisionAttributeEnemyBullet;

	// 描画のためのカメラオブジェクトのポインタがなければ取得しておく
	if (camera_ == nullptr) { camera_ = MainCamera::GetInstance(); }

}

void EnemyBullet::Update() {
	// 早期リターン
	if (!isActive_) { return; }
#ifdef _DEBUG

	ImGui::Begin("EnemyBullet");
	ImGui::SliderAngle("RotateX", &worldTransform_.rotate.x);
	ImGui::SliderAngle("RotateY", &worldTransform_.rotate.y);
	ImGui::SliderAngle("RotateZ", &worldTransform_.rotate.z);
	ImGui::DragFloat3("Rotate", &worldTransform_.rotate.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("Transform", &worldTransform_.translate.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("Velicity", &vel_.x, 0.1f, -100.0f, 100.0f);
	ImGui::End();

#endif // _DEBUG

	// カウントを減少させる
	if (activeCount_ > 0) {
		activeCount_--;
	}
	// カウント0で消失
	if (activeCount_ <= 0) {
		isActive_ = false;
	}

	if (isActive_) {
		// ホーミング弾の処理

		// 敵弾からプレイヤーへのベクトルを計算
		Vec3 toPlayer = player_->GetWorldPos();
		// ベクトルを正規化
		toPlayer = Normalize(toPlayer);
		vel_ = Normalize(Subtract(player_->GetWorldPos(), enemy_->GetWorldPos()));
		// 球面線形補間(Slerp)により、今の速度とプレイヤーへのベクトルを内挿し、新たな速度にする
		vel_ = Slerp(vel_, toPlayer, 0.75f) * -0.5f;

		// 弾の軸を進行方向に合わせる処理


		// 移動処理
		worldTransform_.translate.x += vel_.x;
		worldTransform_.translate.y += vel_.y;
		worldTransform_.translate.z += vel_.z;
	}


	//worldTransform_.worldM = MakeAffineMatrix(
	//	worldTransform_.scale,
	//	worldTransform_.rotate, {
	//	worldTransform_.translate.x + camera_->GetTransform().translate.x,
	//	worldTransform_.translate.y + camera_->GetTransform().translate.y,
	//	worldTransform_.translate.z + camera_->GetTransform().translate.z
	//	});

	worldTransform_.worldM = MakeAffineMatrix(
		worldTransform_.scale,
		worldTransform_.rotate, {
		worldTransform_.translate.x,
		worldTransform_.translate.y,
		worldTransform_.translate.z
		});
	object_->SetWorldTransform(worldTransform_);

	object_->UpdateWorldMat();
	object_->Update();
}

void EnemyBullet::Draw() {
	if (!isActive_) { return; }
	object_->Draw();
}
