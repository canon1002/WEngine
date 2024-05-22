#include "CameraCommon.h"
#include "GameEngine/Base/WinApp/WinAPI.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"

void CameraCommon::Initialize(WinAPI* winApp){
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

void CameraCommon::Update()
{
	// ビュー行列の更新
	viewMatrix_ = Inverse(worldTransform_->GetWorldMatrix());
	// プロジェクション行列の更新
	projectionMatrix_ = MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_);
	// 上記２つをビュープロジェクション行列に合成
	viewprojectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
	
}
