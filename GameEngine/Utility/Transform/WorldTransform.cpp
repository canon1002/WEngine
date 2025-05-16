#include "WorldTransform.h"
#include <cmath>

WorldTransform::WorldTransform(){
	this->Init();
}

WorldTransform::WorldTransform(Vector3 t){
	this->Init();
	translation = t;
}

WorldTransform::WorldTransform(Vector3 t, Vector3 r){
	this->Init();
	translation = t;
	rotation = r;
}

WorldTransform::WorldTransform(Vector3 t, Vector3 r, Vector3 s){
	translation = t;
	rotation = r;
	scale = s;
	parent = nullptr;
}

WorldTransform::~WorldTransform(){

}

void WorldTransform::Init() {
	scale = { 1.0f,1.0f,1.0f };
	rotation = { 0.0f,0.0f,0.0f };
	translation = { 0.0f,0.0f,0.0f };
	parent = nullptr;
}

Matrix4x4 WorldTransform::GetWorldMatrix() const{
	Matrix4x4 result = MakeAffineMatrix(scale, rotation, translation);

	// 親があれば親のワールド行列を掛ける
	if (parent != nullptr) {
		result = Multiply(result, *parent);
	}
	return result;
}


Matrix4x4 WorldTransform::GetScalingMatrix() const{

	Matrix4x4 result = MakeScaleMatrix(scale);
	return result;
}

Matrix4x4 WorldTransform::GetTranslationMatrix() const{
	
	Matrix4x4 result = MakeTranslateMatrix(translation);
	return result;
}

Vector3 WorldTransform::GetWorldPosition()const {
	// ワールド行列を生成
	Matrix4x4 worldMatrix = GetWorldMatrix();
	return Vector3(worldMatrix.m[3][0], worldMatrix.m[3][1], worldMatrix.m[3][2]);
}

Vector3 WorldTransform::GetWorldRotatiton() const
{
	// ワールド行列を取得
	Matrix4x4 matrix = GetWorldMatrix();

	// スケールを抽出
	Vector3 col0(matrix.m[0][0], matrix.m[1][0], matrix.m[2][0]);
	Vector3 col1(matrix.m[0][1], matrix.m[1][1], matrix.m[2][1]);
	Vector3 col2(matrix.m[0][2], matrix.m[1][2], matrix.m[2][2]);

	//Vector3 scale = { 1.0f,1.0f,1.0f };
	//scale.x = Length(col0);
	//scale.y = Length(col1);
	//scale.z = Length(col2);

	// スケール成分を取り除いて回転行列を正規化
	col0 = Normalize(col0);
	col1 = Normalize(col1);
	col2 = Normalize(col2);

	// 回転行列からEuler角を計算
	Vector3 rot = { 0.0f,0.0f,0.0f };
	rot.y = std::asin(-col0.z);
	if (std::cos(rot.y) != 0) {
		rot.x = std::atan2(col1.z, col2.z);
		rot.z = std::atan2(col0.y, col0.x);
	}
	else {
		rot.x = std::atan2(-col2.x, col1.y);
		rot.z = 0;
	}

	return rot;
}

void WorldTransform::SetParent(const Matrix4x4& p){
	// 親のポインタをセット
	parent = nullptr;
	parent = &p;
}

