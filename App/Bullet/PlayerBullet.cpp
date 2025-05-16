#include "PlayerBullet.h"
#include "GameEngine/Editor/BlackBoard.h"
#include "GameEngine/Component/Collider/SphereCollider.h"
#include <numbers>

PlayerBullet::PlayerBullet(const BulletStatus& bulletStatus) {

	// 引数のパラメータをそのまま代入する
	mBulletStatus = bulletStatus;
	// デスフラグをfalseで初期化
	mIsDead = false;
	// 初期化もしておく
	this->Init();

}

void PlayerBullet::Init() {

	// モデルの読み込み
	ModelManager::GetInstance()->LoadModel("PlayerBullet", "PlayerBullet.gltf");

	// オブジェクトの初期化
	mObject = std::make_unique<Object3d>();
	mObject->Init("PlayerBullet");
	// 座標を代入する
	mObject->mWorldTransform->scale = Vector3(mBulletStatus.radius, mBulletStatus.radius, mBulletStatus.radius);
	mObject->mWorldTransform->translation = mBulletStatus.pos;
	// 進行方向を向かせる
	Vector3 normalDir = Normalize(mBulletStatus.direction);
	mObject->mWorldTransform->rotation.y = std::atan2f(normalDir.x, -normalDir.z);
	

	// モデルを設定
	mObject->SetModel("PlayerBullet.gltf");
	// スキニング
	mObject->mSkinning = std::make_unique<Skinning>();
	mObject->mSkinning->Init("PlayerBullet", "PlayerBullet.gltf",mObject->GetModel()->mModelData);
	mObject->mSkinning->IsInactive();

	// 武器にコライダーをセットする
	for (int32_t i = 0; i < mColliders.size(); i++) {
		mBoneWorldMat[i] = MakeAffineMatrix(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f });
		// コライダー 宣言
		std::shared_ptr<GameCollider> newCollider = std::make_shared<GameCollider>();
		newCollider->collider = std::make_unique<SphereCollider>(new WorldTransform(), mBulletStatus.radius * 0.75f);
		// 初期化
		newCollider->collider->Init();
		newCollider->collider->SetTypeID(static_cast<uint32_t>(mBulletStatus.collisionAttribute));
		// 武器に設定したボーンのワールド座標をセット
		newCollider->collider->GetWorld()->SetParent(mBoneWorldMat[i]);
		// 配列にプッシュする
		mColliders[i] = std::move(newCollider);
	}

}

void PlayerBullet::Update(){

	// 時間の加算
	mBulletStatus.currentTime += BlackBoard::GetBattleFPS();
	// 生存時間を過ぎたら消滅
	if (mBulletStatus.lifeTime <= mBulletStatus.currentTime) {
		mIsDead = true;
	}

	// 対象と衝突していたら消滅
	for (auto& collider : mColliders) {
		if (collider->collider->GetOnCollisionFlag()) {
			mIsDead = true;
		}
	}

	// 座標の移動
	mObject->mWorldTransform->translation += Scalar(mBulletStatus.moveSpeed, mBulletStatus.direction);

	// 地面衝突or範囲外での消滅判定
	if (mObject->GetWorldTransform()->GetWorldPosition().y < 0.0f) {
		mIsDead = true;
	}

	// オブジェクト更新
	mObject->Update();

	// 行列更新
	mBoneWorldMat[0] = Multiply(mObject->mSkinning->GetBoneMatrix("Upper0"),mObject->GetWorldTransform()->GetWorldMatrix());
	mBoneWorldMat[1] = Multiply(mObject->mSkinning->GetBoneMatrix("Upper1"),mObject->GetWorldTransform()->GetWorldMatrix());
	mBoneWorldMat[2] = Multiply(mObject->mSkinning->GetBoneMatrix("Upper2"),mObject->GetWorldTransform()->GetWorldMatrix());
	mBoneWorldMat[3] = Multiply(mObject->mSkinning->GetBoneMatrix("Bottom0"),mObject->GetWorldTransform()->GetWorldMatrix());
	mBoneWorldMat[4] = Multiply(mObject->mSkinning->GetBoneMatrix("Bottom1"),mObject->GetWorldTransform()->GetWorldMatrix());
	mBoneWorldMat[5] = Multiply(mObject->mSkinning->GetBoneMatrix("Bottom2"),mObject->GetWorldTransform()->GetWorldMatrix());
	
	// 更新
	for (auto& collider : mColliders) {
		collider->collider->Update();
	}
	
}
