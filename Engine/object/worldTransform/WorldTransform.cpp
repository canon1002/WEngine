#include "WorldTransform.h"
#include "../camera/MatrixCamera.h"

// コンストラクタ
WorldTransform::WorldTransform() {
	// 拡大縮小
	scale_ = { 1.0f,1.0f,1.0f };
	// 回転
	rotate_ = { 0.0f,0.0f,0.0f };
	// 平行移動
	translate_ = { 0.0f,0.0f,0.0f };
}
WorldTransform::WorldTransform(Vector3 t) {
	// 拡大縮小
	scale_ = { 1.0f,1.0f,1.0f };
	// 回転
	rotate_ = { 0.0f,0.0f,0.0f };
	// 平行移動
	translate_ = t;
}
WorldTransform::WorldTransform(Vector3 r, Vector3 t) {
	// 拡大縮小
	scale_ = { 1.0f,1.0f,1.0f };
	// 回転
	rotate_ = r;
	// 平行移動
	translate_ = t;
}
WorldTransform::WorldTransform(Vector3 s, Vector3 r, Vector3 t) {
	// 拡大縮小
	scale_ = s;
	// 回転
	rotate_ = r;
	// 平行移動
	translate_ = t;
}

// デストラクタ
WorldTransform::~WorldTransform() {}

// 行列の更新
void WorldTransform::UpdateMatrix(MatrixCamera& mainCamera)
{
	// 行列を書き込む
	*(constMap) = W::Math::MakeAffineMatrix(scale_, rotate_, translate_);
}

void WorldTransform::CreateTransformationResource(DirectXCommon& dx) {

	// Transformation用のResourceを作る
	constBuffer_ = dx.CreateBufferResource(dx.device.Get(), sizeof(Matrix4x4));
	// データを書き込む
	// 書き込むためのアドレスを取得
	constBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&constMap));
	// 行列を書き込む
	*constMap = W::Math::MakeAffineMatrix(scale_, rotate_, translate_);
	
}