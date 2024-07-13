#include "Arrow.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Append/Collider/AABBCollider.h"
#include "GameEngine/Append/Collider/RayCollider.h"

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
