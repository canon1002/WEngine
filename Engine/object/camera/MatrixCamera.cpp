#include "MatrixCamera.h"

MatrixCamera* MatrixCamera::instance = nullptr;

MatrixCamera::MatrixCamera() { this->Initialize(); }

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

	m_worldMatrix = W::Math::MakeAffineMatrix(); 
	m_cameraMatrix = W::Math::MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.translate);
	m_viewMatrix = W::Math::Inverse(m_cameraMatrix);
	m_projectionMatrix = W::Math::MakePerspectiveMatrix(m_VerticalFOV, m_aspectRatio, m_nearClip, m_farClip);
	m_viewprojectionMatrix = W::Math::Multiply(m_viewMatrix, m_projectionMatrix);
	m_worldViewProjectionMatrix = W::Math::Multiply(m_worldMatrix, m_viewprojectionMatrix);
	m_viewportMatrix = W::Math::MakeViewportMatrix(0, 0, m_windowSize.x, m_windowSize.y, 0.0f, 1.0f);

}



void MatrixCamera::Update() 
{
	m_worldMatrix = W::Math::MakeAffineMatrix(m_scale, m_rotate,m_translate);
	m_cameraMatrix = W::Math::MakeAffineMatrix(m_cameraScale, m_cameraRotate, m_cameraTranslate);
	m_viewMatrix = W::Math::Inverse(m_cameraMatrix);
	m_projectionMatrix = W::Math::MakePerspectiveMatrix(m_VerticalFOV, m_aspectRatio, m_nearClip, m_farClip);
	m_viewprojectionMatrix = W::Math::Multiply(m_viewMatrix, m_projectionMatrix);
	m_worldViewProjectionMatrix = W::Math::Multiply(m_worldMatrix, m_viewprojectionMatrix);
	m_viewportMatrix = W::Math::MakeViewportMatrix(0, 0, m_windowSize.x, m_windowSize.y, 0.0f, 1.0f);
}


Vector3 MatrixCamera::GetNdcPos(Vector3 local)
{
	// ローカルからNDC座標系に変換
	Vector3 ndc = W::Math::Transform(local, m_worldViewProjectionMatrix);
	
	return ndc;
}


Vector4 MatrixCamera::GetNdcPos(Vector4 local)
{
	// ローカルからNDC座標系に変換
	Vector4 ndc = W::Math::Transform(local, m_worldViewProjectionMatrix);

	return ndc;
}

Vector3 MatrixCamera::GetScreenPos(Vector3 ndc)
{
	// スクリーン座標系へ変換
	Vector3 screen = W::Math::Transform(ndc, m_viewportMatrix);

	return screen;
}

Vector4 MatrixCamera::GetScreenPos(Vector4 ndc)
{
	// スクリーン座標系へ変換
	Vector4 screen = W::Math::Transform(ndc, m_viewportMatrix);

	return screen;
}

Matrix4x4 MatrixCamera::GetViewMatrix() {
	return m_viewMatrix;
}


Matrix4x4 MatrixCamera::GetViewportMatrix() {
	return m_viewportMatrix;
}


Matrix4x4 MatrixCamera::GetViewprojectionMatrix() {
	return m_viewprojectionMatrix;
}

void MatrixCamera::SetWorldAffine(Vector3 scale, Vector3 rotate, Vector3 translate) 
{
	m_scale = scale;
	m_rotate = rotate;
	m_translate = translate;
}

void MatrixCamera::SetCameraAffine(Vector3 scale, Vector3 rotate, Vector3 translate) 
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