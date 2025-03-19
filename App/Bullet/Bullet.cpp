#include "Bullet.h"
#include "App/BlackBoard.h"
#include "GameEngine/Append/Collider/SphereCollider.h"

Bullet::Bullet(const BulletStatus& bulletStatus){
	
	// 引数のパラメータをそのまま代入する
	mBulletStatus = bulletStatus;
	// デスフラグをfalseで初期化
	mIsDead = false;

	// 初期化もしておく
	Init();

}

void Bullet::Init(){

	// オブジェクトの初期化
	mObject = std::make_unique<Object3d>();
	mObject->Init("Bullet");
	// 座標を代入する
	mObject->mWorldTransform->scale = Vector3(mBulletStatus.radius, mBulletStatus.radius, mBulletStatus.radius);
	mObject->mWorldTransform->translation = mBulletStatus.pos;

	// モデルを設定
	mObject->SetModel("Sphere.gltf");
	mObject->GetModel()->mMaterialData->color = { 1.0f,0.2f,0.2f,1.0f };

	// コライダーの初期化
	mObject->mCollider = std::make_unique<SphereCollider>(mObject->mWorldTransform.get(), mBulletStatus.radius);
	mObject->mCollider->Init();
	mObject->mCollider->SetCollisionAttribute(mBulletStatus.collisionAttribute);
	mObject->mCollider->SetCollisionMask(mBulletStatus.collisionMask);

}

void Bullet::Update(){

	// 時間の加算
	mBulletStatus.currentTime += BlackBoard::GetBattleFPS();
	// 生存時間を過ぎたら消滅
	if (mBulletStatus.lifeTime <= mBulletStatus.currentTime) {
		mIsDead = true;
	}

	// 対象と衝突していたら消滅
	if (mObject->mCollider->GetOnCollisionFlag()) {
		mIsDead = true;
	}

	// 座標の移動
	mObject->mWorldTransform->translation += Scalar(mBulletStatus.moveSpeed,mBulletStatus.direction);

	// 地面衝突or範囲外での消滅判定
	if (mObject->GetWorldTransform()->GetWorldPosition().y < 0.0f) {
		mIsDead = true;
	}

	// オブジェクト更新
	mObject->Update();
	mObject->mCollider->Update();

}

void Bullet::Draw(){

	// オブジェクト描画
	mObject->Draw();

#ifdef _DEBUG
	// コライダー描画
	mObject->mCollider->Draw();
#endif // _DEBUG

}

bool Bullet::IsDead() const {
	return mIsDead;
}
