#include "MainCamera.h"
#include "Engine/Base/WinAPI.h"
#include "Engine/Base/DirectXCommon.h"

MainCamera* MainCamera::instance = nullptr;

MainCamera::MainCamera() {}

MainCamera* MainCamera::GetInstance()
{
	if (instance == nullptr) {
		instance = new MainCamera();
	}
	return instance;
}

void MainCamera::Finalize()
{
	instance = nullptr;
}

void MainCamera::Initialize(Transformation transform)
{
	transform_ = transform;
	verticalFOV_ = (0.45f);
	aspectRatio_ = (float(WinAPI::GetInstance()->kClientWidth) / float(WinAPI::GetInstance()->kClientHeight));
	nearClip_ = (0.1f);
	farClip_ = (100.0f);
	worldMatrix_ = (MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate));
	viewMatrix_ = (Inverse(worldMatrix_));
	projectionMatrix_ = (MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_));
	viewprojectionMatrix_ = (Multiply(viewMatrix_, projectionMatrix_));
}
void MainCamera::Update()
{
	/*ImGui::Begin("MainCamera");
	ImGui::SliderAngle("RotateX", &transform_.rotate.x);
	ImGui::SliderAngle("RotateY", &transform_.rotate.y);
	ImGui::SliderAngle("RotateZ", &transform_.rotate.z);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("Transform", &transform_.translate.x, 0.1f, -100.0f, 100.0f);
	ImGui::End();*/

	// ワールド行列の計算
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	// ビュー行列の更新
	viewMatrix_ = Inverse(worldMatrix_);
	// プロジェクション行列の更新
	projectionMatrix_ = MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_);
	// 上記２つをビュープロジェクション行列に合成
	viewprojectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);

}
