#include "CameraCommon.h"
#include "GameEngine/Base/WinApp/WinAPI.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"

void CameraCommon::Init(){
	mWorldTransform = std::make_unique<WorldTransform>();
	mVerticalFOV = 0.45f;
	mAspectRatio = (float(WinApp::GetInstance()->kClientWidth) / float(WinApp::GetInstance()->kClientHeight));
	mNearClip = 0.01f;
	mFarClip = 1000.0f;
	mViewMat = Inverse(mWorldTransform->GetWorldMatrix());
	mProjectionMat = MakePerspectiveMatrix(mVerticalFOV, mAspectRatio, mNearClip, mFarClip);
	mViewProjectionMat = Multiply(mViewMat, mProjectionMat);
}

void CameraCommon::Update()
{
	// ビュー行列の更新
	mViewMat = Inverse(mWorldTransform->GetWorldMatrix());
	// プロジェクション行列の更新
	mProjectionMat = MakePerspectiveMatrix(mVerticalFOV, mAspectRatio, mNearClip, mFarClip);
	// 上記２つをビュープロジェクション行列に合成
	mViewProjectionMat = Multiply(mViewMat, mProjectionMat);
	
}
