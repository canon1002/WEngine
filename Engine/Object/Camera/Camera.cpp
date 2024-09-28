#include "Camera.h"
#include "Base/WinApp/WinApp.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::Init(std::string objectName)
{
	WinApp* winApp = WinApp::GetInstance();

	// オブジェクト名を設定
	mObjectName = objectName;

	mWorldTransform = WorldTransform();
	verticalFOV_ = 0.45f;
	aspectRatio_ = (float(winApp->kClientWidth) / float(winApp->kClientHeight));
	nearClip_ = 0.01f;
	farClip_ = 1000.0f;
	mViewMatrix = Inverse(mWorldTransform.GetWorldMatrix());
	mProjectionMatrix_ = MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_);
	mViewprojectionMatrix_ = Multiply(mViewMatrix, mProjectionMatrix_);

}

void Camera::Update()
{

	// ビュー行列の更新
	mViewMatrix = Inverse(mWorldTransform.GetWorldMatrix());
	// プロジェクション行列の更新
	mProjectionMatrix_ = MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_);
	// 上記２つをビュープロジェクション行列に合成
	mViewprojectionMatrix_ = Multiply(mViewMatrix, mProjectionMatrix_);

}

void Camera::Debug()
{
}
