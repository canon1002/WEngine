#include "CameraCommon.h"
#include "GameEngine/Base/WinApp/WinAPI.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"

void CameraCommon::Init(){
	mWorldTransform = std::make_shared<WorldTransform>();
	verticalFOV_ = 0.45f;
	aspectRatio_ = (float(WinApp::GetInstance()->kClientWidth) / float(WinApp::GetInstance()->kClientHeight));
	nearClip_ = 0.01f;
	farClip_ = 1000.0f;
	viewMatrix_ = Inverse(mWorldTransform->GetWorldMatrix());
	projectionMatrix_ = MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_);
	viewprojectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}

void CameraCommon::Update()
{
	// ビュー行列の更新
	viewMatrix_ = Inverse(mWorldTransform->GetWorldMatrix());
	// プロジェクション行列の更新
	projectionMatrix_ = MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_);
	// 上記２つをビュープロジェクション行列に合成
	viewprojectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
	
}
