#include "AABBCollider.h"
#include "SphereCollider.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Model/ModelManager.h"

AABBCollider::AABBCollider(WorldTransform* worldtransform, Vector3 radius) {
	// ワールド座標のポインタを代入
	pWorldTransform = worldtransform;
	// 各ベクトルの半径を代入
	mRadius = radius;
	// 衝突フラグはfalseにしておく
	mIsOnCollision = false;
	// カウントを0で初期化
	mOnCollisionCount = 0;
}

bool AABBCollider::IsCollision(Collider* c) {
	return c->IsCollision(this);
}

bool AABBCollider::IsCollision(AABBCollider* c)
{

	// AABB同士の判定を行う
	if (!((this->GetMin().x <= c->GetMax().x && this->GetMax().x >= c->GetMin().x) &&
		(this->GetMin().y <= c->GetMax().y && this->GetMax().y >= c->GetMin().y) &&
		(this->GetMin().z <= c->GetMax().z && this->GetMax().z >= c->GetMin().z))) {

		// 衝突してなければfalseを返す
		return false;
	}

	// ここから押し出し処理
	//if (Length(this->GetVelocity()) < Length(c->GetVelocity())) {
	//	// 相手側の移動量が多い場合

	//	// 差分の取得

	//	// とりあえず頂点の位置を計算
	//	Vector3 LTF = { c->GetMin().x,c->GetMax().y,c->GetMin().z };
	//	Vector3 RTF = { c->GetMax().x,c->GetMax().y,c->GetMin().z };
	//	Vector3 LBF = { c->GetMin().x,c->GetMin().y,c->GetMin().z };
	//	Vector3 RBF = { c->GetMax().x,c->GetMin().y,c->GetMin().z };
	//	Vector3 LTB = { c->GetMin().x,c->GetMax().y,c->GetMax().z };
	//	Vector3 RTB = { c->GetMax().x,c->GetMax().y,c->GetMax().z };
	//	Vector3 LBB = { c->GetMin().x,c->GetMin().y,c->GetMax().z };
	//	Vector3 RBB = { c->GetMax().x,c->GetMin().y,c->GetMax().z };

	//	// どれがめり込んでいるのかを確認
	//	if ((this->GetMin().x <= LTF.x && this->GetMax().x >= LTF.x) &&
	//		(this->GetMin().y <= LTF.y && this->GetMax().y >= LTF.y) &&
	//		(this->GetMin().z <= LTF.z && this->GetMax().z >= LTF.z)) {



	//	}

	//	Vector3 difference = this->GetWorldPos() - c->GetWorldPos();


	//}

	return true;
}

bool AABBCollider::IsCollision(SphereCollider* c){
	
	Vector3 cloosestPoint{
		std::clamp(c->GetWorldPos().x,this->GetMin().x,this->GetMax().x),
		std::clamp(c->GetWorldPos().y,this->GetMin().y,this->GetMax().y),
		std::clamp(c->GetWorldPos().z,this->GetMin().z,this->GetMax().z)
	};

	float distance = Length(Subtract(cloosestPoint, c->GetWorldPos()));

	if (distance < c->GetRadius()) {
		return true;
	}
	return false;
}

Vector3 AABBCollider::GetMin()const {
	// AABBの最小値を計算する
	Vector3 min = {};
	// ワールド座標 - 各ベクトルの半径
	min = pWorldTransform->translation - mRadius;
	return min;
}

Vector3 AABBCollider::GetMax()const {
	// AABBの最大値を計算する
	Vector3 max = {};
	// ワールド座標 + 各ベクトルの半径
	max = pWorldTransform->translation + mRadius;
	return max;
}

Vector3 AABBCollider::GetVelocity()
{
	Vector3 result = {};
	result = pWorldTransform->translation - mPrePosition;
	return result;
}

void AABBCollider::Init() {
	mDxCommon = DirectXCommon::GetInstance();
	// 矩形のモデルを読み込み // いずれは他の場所に移す
	ModelManager::GetInstance()->LoadModel("wireCube", "wireCube.gltf");
	// モデルを検索してセット
	mModel = ModelManager::GetInstance()->FindModel("wireCube.gltf");
	CreateTransformation();
}

void AABBCollider::Update(){

	// 前フレームの座標を取得
	mPrePosition = pWorldTransform->translation;

	MainCamera* camera = MainCamera::GetInstance();
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = camera->GetViewMatrix();
	// WVPにまとめる
	wvpM = camera->GetViewProjectionMatrix();

	// AABBなので回転をなくしたワールド行列を作る
	WorldTransform world = *pWorldTransform;
	// ridiusに合わせて拡大縮小を行う
	world.scale = { mRadius.x * 2.0f,mRadius.y * 2.0f,mRadius.z * 2.0f };
	world.rotation = { 0.0f,0.0f,0.0f };
	Matrix4x4 worldM = world.GetWorldMatrix();
	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	mWvpData->WVP = Multiply(worldM, wvpM);
	mWvpData->World = worldM;

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

void AABBCollider::Draw(){

	//wvp用のCBufferの場所を指定
	mDxCommon->commandList->SetGraphicsRootConstantBufferView(1, mWvpResource->GetGPUVirtualAddress());
	// 頂点をセット
	// 配列を渡す(開始スロット番号、使用スロット数、VBV配列へのポインタ)
	mDxCommon->commandList->IASetVertexBuffers(0, 1, &mModel->mVertexBufferView);

	mModel->Draw();

}

void AABBCollider::CreateTransformation(){
	// Transformation用のResourceを作る
	mWvpResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(TransformationMatrixForGrid3D));
	// データを書き込む
	// 書き込むためのアドレスを取得
	mWvpResource->Map(0, nullptr, reinterpret_cast<void**>(&mWvpData));
	// 単位行列を書き込む
	mWvpData->WVP = MakeAffineMatrix(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
	mWvpData->World = MakeIdentity();
}