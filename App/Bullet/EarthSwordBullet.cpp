#include "EarthSwordBullet.h"
#include "GameEngine/Editor/BlackBoard.h"
#include "GameEngine/Component/Collider/SphereCollider.h"

EarthSwordBullet::EarthSwordBullet(const BulletStatus& bulletStatus){
	
	// 引数のパラメータをそのまま代入する
	mBulletStatus = bulletStatus;
	// デスフラグをfalseで初期化
	mIsDead = false;
	// 初期化もしておく
	this->Init();

}

void EarthSwordBullet::Init(){

	// オブジェクトの初期化
	mObject = std::make_unique<Object3d>();
	mObject->Init("EarthSwordBullet");
	// 座標を代入する
	mObject->mWorldTransform->scale = Vector3(mBulletStatus.radius, mBulletStatus.radius, mBulletStatus.radius);
	mObject->mWorldTransform->translation = mBulletStatus.pos;

	// モデルを設定
	mObject->SetModel("EarthSwordBullet.gltf");
	mObject->GetModel()->GetMaterialData()->color = { 1.0f,1.0f,1.0f,1.0f };

	// コライダーの初期化
	mCollider = std::make_shared<GameCollider>();
	mCollider->collider = std::make_unique<SphereCollider>(mObject->mWorldTransform.get(), mBulletStatus.radius * 0.75f);
	// 初期化
	mCollider->collider->Init();
	mCollider->collider->SetTypeID(static_cast<uint32_t>(mBulletStatus.collisionAttribute));

}

void EarthSwordBullet::Update(){

	// 時間の加算
	mBulletStatus.currentTime += BlackBoard::GetBattleFPS();
	// 生存時間を過ぎたら消滅
	if (mBulletStatus.lifeTime <= mBulletStatus.currentTime) {
		mIsDead = true;
	}

	// 対象と衝突していたら消滅
	if (mCollider->collider->GetOnCollisionFlag()) {
		mIsDead = true;
	}

	// 座標の移動
	if (mObject->mWorldTransform->translation.y < 0.0f) {
		mObject->mWorldTransform->translation += Scalar(mBulletStatus.moveSpeed, mBulletStatus.direction);
	}

	// オブジェクト更新
	mObject->Update();
	mCollider->collider->Update();

}
