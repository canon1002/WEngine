#include "RailCamera.h"

void RailCamera::Initialize() {

	this->SetRotate(Vec3{0.0f,0.0f,0.0f});
	this->SetTranslate(Vec3{ 0.0f,0.0f,0.0f });

}

void RailCamera::Initialize(Vec3 t, Vec3 r) {

	this->SetRotate(r);
	this->SetTranslate(t);

}


void RailCamera::Update() {

#ifdef _DEBUG

	ImGui::Begin("Camera");
	ImGui::SliderAngle("RotateX", &transform_.rotate.x);
	ImGui::SliderAngle("RotateY", &transform_.rotate.y);
	ImGui::SliderAngle("RotateZ", &transform_.rotate.z);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("Transform", &transform_.translate.x, 0.1f, -100.0f, 100.0f);
	ImGui::End();

#endif // _DEBUG

	// ワールド行列の計算
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	// ビュー行列の更新
	viewMatrix_ = Inverse(worldMatrix_);
	// プロジェクション行列の更新
	projectionMatrix_ = MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_);
	// 上記２つをビュープロジェクション行列に合成
	viewprojectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);


}