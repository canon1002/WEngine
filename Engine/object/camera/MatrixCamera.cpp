#include "MatrixCamera.h"

MatrixCamera* MatrixCamera::instance = nullptr;

MatrixCamera::MatrixCamera(){}

MatrixCamera::~MatrixCamera() {};


/// インスタンス取得
MatrixCamera* MatrixCamera::GetInstance() {
	if (instance == nullptr) {
		instance = new MatrixCamera;
	}
	return instance;
}

void MatrixCamera::Delete() {
	delete instance;
	instance = nullptr;
}

void MatrixCamera::Initialize() {

	m_windowSize = { 1280.0f,720.0f };

#pragma region ワールド座標系

	// 初期設定
	transform_ = { { 1.0f,1.0f,1.0f },{ 0.0f,0.0f,0.0f },{ 0.0f,0.0f,0.0f } };

	// 拡大率
	m_scale = { 1.0f,1.0f,1.0f };
	// 回転量
	m_rotate = { 0.0f,0.0f,0.0f };
	// 移動量
	m_translate = { 0.0f,0.0f,0.0f };

#pragma endregion

#pragma region カメラ座標系

	// カメラの初期設定
	cameraTransform_ = { { 1.0f,1.0f,1.0f },{ 0.0f,0.0f,0.0f },{ 0.0f,0.0f,-5.0f } };
	// Near(近平面への距離)
	m_nearClip = 0.1f;
	// Far(遠平面への距離)
	m_farClip = 100.0f;
	// AspectRatio(アスペクト比)
	m_aspectRatio = (m_windowSize.x / m_windowSize.y);
	// VerticalFOV(縦画角)
	m_VerticalFOV = 0.45f;

#pragma endregion

	m_worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
	m_cameraMatrix = MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.translate);
	m_viewMatrix = Inverse(m_cameraMatrix);
	m_projectionMatrix = MakePerspectiveMatrix(m_VerticalFOV, m_aspectRatio, m_nearClip, m_farClip);
	m_viewprojectionMatrix = Multiply(m_viewMatrix, m_projectionMatrix);
	m_worldViewProjectionMatrix = Multiply(m_worldMatrix, m_viewprojectionMatrix);
	m_viewportMatrix = MakeViewportMatrix(0, 0, m_windowSize.x, m_windowSize.y, 0.0f, 1.0f);

}



void MatrixCamera::Update() 
{
	m_rotate.y += 0.03f;
	m_worldMatrix = MakeAffineMatrix(m_scale, m_rotate,m_translate);
	m_cameraMatrix = MakeAffineMatrix(m_cameraScale, m_cameraRotate, m_cameraTranslate);
	m_viewMatrix = Inverse(m_cameraMatrix);
	m_projectionMatrix = MakePerspectiveMatrix(m_VerticalFOV, m_aspectRatio, m_nearClip, m_farClip);
	m_worldViewProjectionMatrix = Multiply(m_worldMatrix, Multiply(m_viewMatrix, m_projectionMatrix));
	m_viewportMatrix = MakeViewportMatrix(0, 0, m_windowSize.x, m_windowSize.y, 0.0f, 1.0f);
}


Vec3 MatrixCamera::GetNdcPos(Vec3 local)
{
	// ローカルからNDC座標系に変換
	Vec3 ndc = Transform(local, m_worldViewProjectionMatrix);
	
	return ndc;
}


Vec4 MatrixCamera::GetNdcPos(Vec4 local)
{
	// ローカルからNDC座標系に変換
	Vec4 ndc = Transform(local, m_worldViewProjectionMatrix);

	return ndc;
}

Vec3 MatrixCamera::GetScreenPos(Vec3 ndc)
{
	// スクリーン座標系へ変換
	Vec3 screen = Transform(ndc, m_viewportMatrix);

	return screen;
}

Vec4 MatrixCamera::GetScreenPos(Vec4 ndc)
{
	// スクリーン座標系へ変換
	Vec4 screen = Transform(ndc, m_viewportMatrix);

	return screen;
}

Mat44 MatrixCamera::GetViewMatrix() {
	return m_viewMatrix;
}


Mat44 MatrixCamera::GetViewportMatrix() {
	return m_viewportMatrix;
}


Mat44 MatrixCamera::GetViewprojectionMatrix() {
	return m_viewprojectionMatrix;
}

void MatrixCamera::SetWorldAffine(Vec3 scale, Vec3 rotate, Vec3 translate) 
{
	m_scale = scale;
	m_rotate = rotate;
	m_translate = translate;
}

void MatrixCamera::SetCameraAffine(Vec3 scale, Vec3 rotate, Vec3 translate) 
{
	m_cameraScale = scale;
	m_cameraRotate = rotate;
	m_cameraTranslate = translate;
}

void MatrixCamera::SetCrip(float nearCrip, float farClip)
{
	m_nearClip = nearCrip;
	m_farClip = farClip;

}

void MatrixCamera::SetAspectRatio(float aspectRatio)
{
	m_aspectRatio = aspectRatio;
}