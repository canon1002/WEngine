#include "WorldTransform.h"

WorldTransform::WorldTransform(){
	this->Init();
}

WorldTransform::WorldTransform(Vec3 t){
	this->Init();
	translation = t;
}

WorldTransform::WorldTransform(Vec3 t, Vec3 r){
	this->Init();
	translation = t;
	rotation = r;
}

WorldTransform::WorldTransform(Vec3 t, Vec3 r, Vec3 s){
	translation = t;
	rotation = r;
	scale = s;
	parent_ = nullptr;
}

WorldTransform::~WorldTransform(){
	if (parent_ != nullptr) {
		delete parent_;
	}
}

void WorldTransform::Init() {
	scale = { 1.0f,1.0f,1.0f };
	rotation = { 0.0f,0.0f,0.0f };
	translation = { 0.0f,0.0f,0.0f };
	parent_ = nullptr;
}

Mat44 WorldTransform::GetWorldMatrix() const{
	Mat44 result = MakeAffineMatrix(scale, rotation, translation);
	if (parent_ == nullptr) {
		
		return result;
	}
	// 親があれば親のワールド行列を掛ける
	if (parent_ != nullptr) {
		//result = Multiply(result, parent_->GetWorldMatrix());
	}
	return result;
}

Vec3 WorldTransform::GetWorldPosition(){
	// ワールド行列を生成
	Mat44 worldMatrix = GetWorldMatrix();
	return Vec3(worldMatrix.m[3][0], worldMatrix.m[3][1], worldMatrix.m[3][2]);
}

void WorldTransform::SetParent(WorldTransform* parent){
	// 親のポインタをセット
	parent_ = parent;
}
