#include "CameraCommon.h"
#include "Engine/Base/WinAPI.h"
#include "Engine/Base/DirectXCommon.h"

CameraCommon::CameraCommon()
	:transform_({ Vec3{ 1.0f,1.0f,1.0f }, Vec3{ 0.0f,0.0f,0.0f },Vec3 { 0.0f,0.0f,0.0f } })
	, verticalFOV_(0.45f)
	, aspectRatio_(float(WinAPI::GetInstance()->kClientWidth) / float(WinAPI::GetInstance()->kClientHeight))
	,nearClip_(0.1f)
	,farClip_(100.0f)
	, worldMatrix_(MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate))
	,viewMatrix_(Inverse(worldMatrix_))
	, projectionMatrix_(MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_))
	, viewprojectionMatrix_(Multiply(viewMatrix_, projectionMatrix_))
{}

CameraCommon::~CameraCommon()
{
}


void CameraCommon::Initialize(Transformation transform)
{
	transform_ = transform;
}

void CameraCommon::Update()
{
	// ワールド行列の計算
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	// ビュー行列の更新
	viewMatrix_ = Inverse(worldMatrix_);
	// プロジェクション行列の更新
	projectionMatrix_ = MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_);
	// 上記２つをビュープロジェクション行列に合成
	viewprojectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
	
}
