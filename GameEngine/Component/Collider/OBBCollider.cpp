#include "OBBCollider.h"
#include "AABBCollider.h"
#include "SphereCollider.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Resource/Model/ModelManager.h"

OBBCollider::OBBCollider(WorldTransform* worldtransform, Vector3 size) {
	// ワールド座標のポインタを代入
	mWorldTransform = worldtransform;
	// サイズを代入
	mSize = size;
	// 衝突フラグはfalseにしておく
	mIsOnCollision = false;
	// カウントを0で初期化
	mOnCollisionCount = 0;
}

bool OBBCollider::IsCollision(Collider* c) {
	return c->IsCollision(this);
}

bool OBBCollider::IsCollision(AABBCollider* c){
	c;
	//// 分離軸の選定
	//std::vector<Vector3> axes = {
	//	mWorldTransform->GetRight(), mWorldTransform->GetUp(), mWorldTransform->GetForward(),
	//	Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1)
	//};

	//for (const auto& axis : axes) {
	//	if (!OverlapOnAxis(this, aabb, axis)) {
	//		return false;
	//	}
	//}

	return true;
}

bool OBBCollider::IsCollision(SphereCollider* c) {

	// 中心点をOBBのローカル空間上の点にする
	Vector3 centerInOBBLocalSpace =
		Transform(c->GetWorldPos(), Inverse(mWorldTransform->GetWorldMatrix()));

	// OBBからABBを作成
	AABB aabbOBBLocal = {};
	aabbOBBLocal.min = mSize * (-1.0f);
	aabbOBBLocal.max = mSize;

	Sphere sphereOBBLocal = {};
	sphereOBBLocal.center = centerInOBBLocalSpace;
	sphereOBBLocal.radius = c->GetRadius();

	// ローカル空間での球とAABBの衝突判定
	Vector3 cloosestPoint{
	std::clamp(sphereOBBLocal.center.x,aabbOBBLocal.min.x,aabbOBBLocal.max.x),
	std::clamp(sphereOBBLocal.center.y,aabbOBBLocal.min.y,aabbOBBLocal.max.y),
	std::clamp(sphereOBBLocal.center.z,aabbOBBLocal.min.z,aabbOBBLocal.max.z),
	};

	float distance = Length(Subtract(cloosestPoint, sphereOBBLocal.center));
	if (distance < sphereOBBLocal.radius) {
		return true;
	}
	return false;
}

bool OBBCollider::IsCollision(OBBCollider* c){

	// 分離軸の選定
	std::vector<Vector3> axes; 
	axes.push_back(Vector3(this->GetLen().x, this->GetLen().y, this->GetLen().z));
	axes.push_back(Vector3(c->GetLen().x, c->GetLen().y, c->GetLen().z));

	for (const auto& axis : axes) {
		if (!OverlapOnAxis(this, c, axis)) {
			return false;
		}
	}

	return true;
}


bool OBBCollider::OverlapOnAxis(OBBCollider* a, OBBCollider* b, const Vector3& axis) {
	// OBBの頂点を軸に投影し、範囲を計算
	float minA, maxA, minB, maxB;
	ProjectOBB(a, axis, minA, maxA);
	ProjectOBB(b, axis, minB, maxB);

	// 範囲の重なり確認
	return (minA <= maxB && minB <= maxA);
}

void OBBCollider::ProjectOBB(OBBCollider* obb, const Vector3& axis, float& min, float& max) {
	// OBBの頂点を軸に投影
	std::vector<Vector3> vertices = obb->GetVertices();
	min = max = Dot(vertices[0], axis);

	for (const auto& vertex : vertices) {
		float projection = Dot(vertex, axis);
		if (projection < min) min = projection;
		if (projection > max) max = projection;
	}
}

bool OBBCollider::OverlapOnAxis(OBBCollider* obb, AABBCollider* aabb, const Vector3& axis) {
	// OBBとAABBの頂点を軸に投影し、範囲を計算
	float minOBB, maxOBB, minAABB, maxAABB;
	ProjectOBB(obb, axis, minOBB, maxOBB);
	ProjectAABB(aabb, axis, minAABB, maxAABB);

	// 範囲の重なり確認
	return (minOBB <= maxAABB && minAABB <= maxOBB);
}

void OBBCollider::ProjectAABB(AABBCollider* aabb, const Vector3& axis, float& min, float& max) {
	// AABBの頂点を軸に投影
	std::vector<Vector3> vertices = aabb->GetVertices();
	min = max = Dot(vertices[0], axis);

	for (const auto& vertex : vertices) {
		float projection = Dot(vertex, axis);
		if (projection < min) min = projection;
		if (projection > max) max = projection;
	}
}

Vector3 OBBCollider::GetVelocity()
{
	Vector3 result = {};
	result = mWorldTransform->translation - mPrePosition;
	return result;
}

void OBBCollider::Init() {
	// 矩形のモデルを読み込み // いずれは他の場所に移す
	ModelManager::GetInstance()->LoadModel("wireCube", "wireCube.gltf");
	// モデルを検索してセット
	mModel = ModelManager::GetInstance()->FindModel("wireCube.gltf");
	CreateTransformation();
	// 衝突フラグ保持時間
	mOnCollisionCount = 0;
	// フラグをfalseに
	mIsOnCollision = false;
}

void OBBCollider::Update() {

	MainCamera* camera = MainCamera::GetInstance();
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = camera->GetViewMatrix();
	// WVPにまとめる
	wvpM = camera->GetViewProjectionMatrix();

	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	mWvpData->WVP = Multiply(mWorldTransform->GetWorldMatrix(), wvpM);
	mWvpData->World = mWorldTransform->GetWorldMatrix();

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

void OBBCollider::Draw() {

	//wvp用のCBufferの場所を指定
	//DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(1, mWvpResource->GetGPUVirtualAddress());
	//mModel->Draw();

}

void OBBCollider::CreateTransformation() {
	// Transformation用のResourceを作る
	mWvpResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(TransformationMatrixForGrid3D));
	// データを書き込む
	// 書き込むためのアドレスを取得
	mWvpResource->Map(0, nullptr, reinterpret_cast<void**>(&mWvpData));
	// 単位行列を書き込む
	mWvpData->WVP = MakeAffineMatrix(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
	mWvpData->World = MakeIdentity();
}
