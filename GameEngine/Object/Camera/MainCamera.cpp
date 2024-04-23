#include "MainCamera.h"
#include "GameEngine/Base/WinApp/WinAPI.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"


MainCamera* MainCamera::instance = nullptr;


MainCamera* MainCamera::GetInstance(){
	if (instance == nullptr) {
		instance = new MainCamera();
	}
	return instance;
}

void MainCamera::Initialize(WinAPI* winApp){
	winApp_ = winApp;
	worldTransform_ = new WorldTransform();
	verticalFOV_ = 0.45f;
	aspectRatio_ = (float(winApp->kClientWidth) / float(winApp->kClientHeight));
	nearClip_ = 0.1f;
	farClip_ = 100.0f;
	viewMatrix_ = Inverse(worldTransform_->GetWorldMatrix());
	projectionMatrix_ = MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_);
	viewprojectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}
void MainCamera::Update()
{
	ImGui::Begin("MainCamera");
	ImGui::SliderAngle("RotateX", &worldTransform_->rotation.x);
	ImGui::SliderAngle("RotateY", &worldTransform_->rotation.y);
	ImGui::SliderAngle("RotateZ", &worldTransform_->rotation.z);
	ImGui::DragFloat3("Rotate", &worldTransform_->rotation.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("Transform", &worldTransform_->translation.x, 0.1f, -100.0f, 100.0f);
	ImGui::End();

	// ビュー行列の更新
	viewMatrix_ = Inverse(worldTransform_->GetWorldMatrix());
	// プロジェクション行列の更新
	projectionMatrix_ = MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_);
	// 上記２つをビュープロジェクション行列に合成
	viewprojectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);


}
