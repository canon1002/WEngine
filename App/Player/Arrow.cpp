#include "Arrow.h"
#include "Resource/Model/ModelManager.h"

#include "Editor/ImGui/ImGuiManager.h"
#include "Component/Collider/AABBCollider.h"
#include "Component/Collider/SphereCollider.h"
#include <numbers>
#include "App/Reaction/DamageReaction.h"
#include "App/Status/StatusManager.h"
#include "App/Player/Player.h"
#include "App/Enemy/BossEnemy.h"

Arrow::Arrow() {}
Arrow::~Arrow() {

}


void Arrow::Init(Player* player,Vector3 pos, Vector3 vel)
{
	mPlayer = player;

	mObject = std::make_unique<Object3d>();
	mObject->Init("Arrow");
	mObject->SetModel("Arrow.gltf");

	// 拡大率を変更
	mObject->mWorldTransform.scale = { 0.1f,0.1f,1.0f };
	// 代入した数値を初期座標にする
	mObject->mWorldTransform.translation = pos;

	// 移動量を設定
	mMoveVel = vel;

	////回転
	//Vector3 rotate = { 0.0f,0.0f,0.0f };
	//const float PI = float(std::numbers::pi);
	//rotate.y = std::atan2f(mMoveVel.x, mMoveVel.z);
	//rotate.y = std::fmodf(rotate.y, 2.0f * PI);
	//// 回転量が超過していたり、下限を下回っていたら補正する
	//if (rotate.y > PI) { rotate.y -= 2.0f * PI; }
	//if (rotate.y < -PI) { rotate.y += 2.0f * PI; }
	//mObject->SetRotate(rotate);

	//コライダーの宣言
	// コライダーの宣言
	mObject->mCollider = new SphereCollider(&mObject->mWorldTransform, 0.05f);
	mObject->mCollider->Init();
	mObject->mCollider->SetCollisionAttribute(kCollisionAttributePlayerBullet);
	mObject->mCollider->SetCollisionMask(kCollisionAttributeEnemy);

	// フラグをtrueにする
	mIsActive = true;
}

void Arrow::Update()
{
	// フラグがfalseであれば早期リターン
	if (mIsActive == false) { return; }

	// 移動させる
	mObject->AddTranslate(mMoveVel);

	if (mObject->mWorldTransform.translation.x >= 250.0f ||
		mObject->mWorldTransform.translation.x <= -250.0f ||
		mObject->mWorldTransform.translation.y >= 250.0f ||
		mObject->mWorldTransform.translation.y <= -250.0f ||
		mObject->mWorldTransform.translation.z >= 250.0f ||
		mObject->mWorldTransform.translation.z <= -250.0f) {
		mIsActive = false;
	}

	// 衝突時の処理
	if (mObject->mCollider->GetOnCollisionFlag()) {
		// 次のフレームで消す
		mIsActive = false;

		// 敵にダメージを与える
		mPlayer->ReciveDamageToBoss(1.2f);

		// ダメージ表示
		int32_t damage = static_cast<int32_t>(static_cast<int32_t>(mPlayer->GetStatus()->STR / 2.0f) * 1.0f) - static_cast<int32_t>(mPlayer->mBoss->GetStatus()->VIT / 4.0f);;
		DamageReaction::GetInstance()->Reaction(this->GetWorldPos(), damage, *MainCamera::GetInstance());
	}

	mObject->Update();
	mObject->mCollider->Update();
}

void Arrow::Draw(){
	mObject->Draw(*MainCamera::GetInstance());
}
