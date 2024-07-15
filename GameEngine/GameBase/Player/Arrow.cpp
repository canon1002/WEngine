#include "Arrow.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Append/Collider/AABBCollider.h"
#include "GameEngine/Append/Collider/RayCollider.h"
#include <numbers>

Arrow::Arrow() {}
Arrow::~Arrow() {}


void Arrow::Init(Vector3 pos, Vector3 vel)
{
	mObject = std::make_unique<Object3d>();
	mObject->Init("Arrow");
	mObject->SetModel("Arrow.gltf");

	// 代入した数値を初期座標にする
	mObject->SetTranslate(pos);

	// 移動量を設定
	mMoveVel = vel;

	//回転
	Vector3 rotate = { 0.0f,0.0f,0.0f };
	const float PI = float(std::numbers::pi);
	rotate.y = std::atan2f(mMoveVel.x, mMoveVel.z);
	rotate.y = std::fmodf(rotate.y, 2.0f * PI);
	// 回転量が超過していたり、下限を下回っていたら補正する
	if (rotate.y > PI) { rotate.y -= 2.0f * PI; }
	if (rotate.y < -PI) { rotate.y += 2.0f * PI; }
	mObject->SetRotate(rotate);

	//コライダーの宣言
	//mObject->mCollider = new AABBCollider(mObject->mWorldTransform, Vector3(0.1f, 0.1f, 1.0f));

	// フラグをtrueにする
	mIsActive = true;
}

void Arrow::Update()
{
	// フラグがfalseであれば早期リターン
	if (mIsActive == false) { return; }

	// 移動させる
	mObject->SetTranslate(mObject->GetWorldTransform()->translation + mMoveVel);

	if (mObject->GetWorldTransform()->translation.x >= 50.0f ||
		mObject->GetWorldTransform()->translation.x <= -50.0f ||
		mObject->GetWorldTransform()->translation.y >= 50.0f ||
		mObject->GetWorldTransform()->translation.y <= -50.0f ||
		mObject->GetWorldTransform()->translation.z >= 50.0f ||
		mObject->GetWorldTransform()->translation.z <= -50.0f) {
		mIsActive = false;
	}

	mObject->Update();

}

void Arrow::Draw(){
	mObject->Draw();
}
