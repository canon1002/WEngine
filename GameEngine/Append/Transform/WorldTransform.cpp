#include "WorldTransform.h"

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
	parent_ = nullptr;
}

WorldTransform::~WorldTransform(){

}

void WorldTransform::Init() {
	scale = { 1.0f,1.0f,1.0f };
	rotation = { 0.0f,0.0f,0.0f };
	translation = { 0.0f,0.0f,0.0f };
	parent_ = nullptr;
}

Matrix4x4 WorldTransform::GetWorldMatrix() const{
	Matrix4x4 result = MakeAffineMatrix(scale, rotation, translation);

	// 親があれば親のワールド行列を掛ける
	if (parent_ != nullptr) {
		result = Multiply(result, *parent_);
	}
	return result;
}

Vector3 WorldTransform::GetWorldPosition(){
	// ワールド行列を生成
	Matrix4x4 worldMatrix = GetWorldMatrix();
	return Vector3(worldMatrix.m[3][0], worldMatrix.m[3][1], worldMatrix.m[3][2]);
}

void WorldTransform::SetParent(const Matrix4x4& parent){
	// 親のポインタをセット
	parent_ = &parent;
}

