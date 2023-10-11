#pragma once
#include"Math.h"

class MatrixCamera
{
public:	// メンバ関数

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="windowSize"></param>
	MatrixCamera(Vector2 windowSize);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~MatrixCamera();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

#pragma region セット関数

	/// <summary>
	/// ワールドのアフィン行列を設定
	/// </summary>
	/// <param name="scale"></param>
	/// <param name="rotate"></param>
	/// <param name="translate"></param>
	void SetWorldAffine(Vector3 scale, Vector3 rotate, Vector3 translate);


	/// <summary>
	/// カメラのアフィン行列を設定
	/// </summary>
	/// <param name="scale"></param>
	/// <param name="rotate"></param>
	/// <param name="translate"></param>
	void SetCameraAffine(Vector3 scale, Vector3 rotate, Vector3 translate);


	/// <summary>
	/// クリップを設定
	/// </summary>
	/// <param name="nearCrip"></param>
	/// <param name="farClip"></param>
	void SetCrip(float nearCrip, float farClip);


	/// <summary>
	/// アスペクト比の設定
	/// </summary>
	/// <param name="aspectRatio"></param>
	void SetAspectRatio(float aspectRatio);

#pragma endregion


#pragma region ゲット関数

	Vector3 GetNdcPos(Vector3 local);
	Vector4 GetNdcPos(Vector4 local);

	Vector3 GetScreenPos(Vector3 ndc);
	Vector4 GetScreenPos(Vector4 ndc);

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	Matrix4x4 GetViewMatrix();

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	Matrix4x4 GetViewportMatrix();

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	Matrix4x4 GetViewprojectionMatrix();

	const Matrix4x4& GetWorldViewProjection() { return m_worldViewProjectionMatrix; }

#pragma endregion



private: // メンバ変数

#pragma region ワールド座標系

	Transform transform_;
	// 拡大率
	Vector3 m_scale;
	// 回転量
	Vector3 m_rotate;
	// 移動量
	Vector3 m_translate;
	// 画面サイズ
	Vector2 m_windowSize;

#pragma endregion

#pragma region カメラ座標系

	//
	Transform cameraTransform_;

	// 拡大率
	Vector3 m_cameraScale;
	// 回転量
	Vector3 m_cameraRotate;
	// 移動量
	Vector3 m_cameraTranslate;
	// Near(近平面への距離)
	float m_nearClip;
	// Far(遠平面への距離)
	float m_farClip;
	// AspectRatio(アスペクト比)
	float m_aspectRatio;
	// VerticalFOV(縦画角)
	float m_VerticalFOV;

#pragma endregion

#pragma region 行列

	Matrix4x4 m_worldMatrix;
	Matrix4x4 m_cameraMatrix;
	Matrix4x4 m_viewMatrix;
	Matrix4x4 m_projectionMatrix;
	Matrix4x4 m_viewprojectionMatrix;
	Matrix4x4 m_worldViewProjectionMatrix;
	Matrix4x4 m_viewportMatrix;

#pragma endregion


};