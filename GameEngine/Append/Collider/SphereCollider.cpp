#include "SphereCollider.h"
#include "AABBCollider.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"

SphereCollider::SphereCollider(WorldTransform* worldtransform, float radius){
	// ワールド座標のポインタを代入
	pWorldTransform = worldtransform;
	// 半径を代入
	mRadius = radius;
	// 追加平行移動の初期化
	mAddtranslation = { 0.0f, 0.0f, 0.0f };
}


void SphereCollider::Init() {
	mDxCommon = DirectXCommon::GetInstance();
	// 矩形のモデルを読み込み // いずれは他の場所に移す
	ModelManager::GetInstance()->LoadModel("wireSphere", "wireSphere.gltf");
	// モデルを検索してセット
	mModel = ModelManager::GetInstance()->FindModel("wireSphere.gltf");
	CreateTransformation();
	mModel->mMaterialData->environmentCoefficient = 0.0f;
	// 衝突フラグ保持時間
	mOnCollisionCount = 0;
	// フラグをfalseに
	mIsOnCollision = false;
}

void SphereCollider::Update() {

	MainCamera* camera = MainCamera::GetInstance();
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = camera->GetViewMatrix();
	// WVPにまとめる
	wvpM = camera->GetViewProjectionMatrix();
	// ワールド行列を作る
	WorldTransform* world = new WorldTransform();
	world->translation = pWorldTransform->GetWorldPosition();
	// ridiusに合わせて拡大縮小を行う
	world->scale = { mRadius,mRadius,mRadius};
	// 追加移動量の適用
	world->translation += mAddtranslation;
	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	mWvpData->WVP = Multiply(world->GetWorldMatrix(), wvpM);
	mWvpData->World = world->GetWorldMatrix();
	// 更新
	mModel->Update();
	// カウント減少
	if (mOnCollisionCount > 0) {
		mOnCollisionCount--;
		if (mOnCollisionCount <= 0) {
			mOnCollisionCount = 0;
			// フラグをfalseに
			mIsOnCollision = false;
		}
	}

}

void SphereCollider::Draw() {
	//wvp用のCBufferの場所を指定
	mDxCommon->mCommandList->SetGraphicsRootConstantBufferView(1, mWvpResource->GetGPUVirtualAddress());
	// 頂点をセット
	// 配列を渡す(開始スロット番号、使用スロット数、VBV配列へのポインタ)
	mDxCommon->mCommandList->IASetVertexBuffers(0, 1, &mModel->mVertexBufferView);
	// 描画
	mModel->Draw();
}

void SphereCollider::CreateTransformation() {
	// Transformation用のResourceを作る
	mWvpResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(TransformationMatrixForGrid3D));
	// データを書き込む
	// 書き込むためのアドレスを取得
	mWvpResource->Map(0, nullptr, reinterpret_cast<void**>(&mWvpData));
	// 単位行列を書き込む
	mWvpData->WVP = MakeAffineMatrix(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
	mWvpData->World = MakeIdentity();
}


bool SphereCollider::IsCollision(Collider* c) {
	return c->IsCollision(this);
}

bool SphereCollider::IsCollision(AABBCollider* c)
{
	// AABBとの判定
	Vector3 cloosestPoint{
		clamp(this->GetWorldPos().x,c->GetMin().x,c->GetMax().x),
		clamp(this->GetWorldPos().y,c->GetMin().y,c->GetMax().y),
		clamp(this->GetWorldPos().z,c->GetMin().z,c->GetMax().z)
	};

	float distance = Length(Subtract(cloosestPoint, this->GetWorldPos()));

	if (distance < this->GetRadius()) {
		return true;
	}
	return false;
}

bool SphereCollider::IsCollision(SphereCollider* c) {

	// 2つの球体の中心点間の距離を求める
	Vector3 collisionDistance = Subtract(this->GetWorldPos(), c->GetWorldPos());

	//距離を計算
	float distance = sqrtf(
		(collisionDistance.x * collisionDistance.x) +
		(collisionDistance.y * collisionDistance.y) +
		(collisionDistance.z * collisionDistance.z));

	// 半径の合計よりも短ければ衝突
	float radius = (this->GetRadius() + c->GetRadius()) * (this->GetRadius() + c->GetRadius());
	if (distance <= radius)
	{
		return true;
	}
	return false;
}
