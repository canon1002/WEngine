#pragma once
#include"../../math/Math.h"
#include"../worldTransform/WorldTransform.h"

class MatrixCamera
{

	/// <summary>
	/// コンストラクタ
	/// </summary>
	MatrixCamera();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~MatrixCamera();

public:	// メンバ関数
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static MatrixCamera* GetInstance();

	/// <summary>
	/// 
	/// </summary>
	static void Delete();


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
	void SetWorldAffine(Vec3 scale, Vec3 rotate, Vec3 translate);


	/// <summary>
	/// カメラのアフィン行列を設定
	/// </summary>
	/// <param name="scale"></param>
	/// <param name="rotate"></param>
	/// <param name="translate"></param>
	void SetCameraAffine(Vec3 scale, Vec3 rotate, Vec3 translate);


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

	Vec3 GetNdcPos(Vec3 local);
	Vec4 GetNdcPos(Vec4 local);

	Vec3 GetScreenPos(Vec3 ndc);
	Vec4 GetScreenPos(Vec4 ndc);

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	Mat44 GetViewMatrix();

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	Mat44 GetViewportMatrix();

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	Mat44 GetViewprojectionMatrix();

	const Mat44& GetWorldViewProjection() { return m_worldViewProjectionMatrix; }

#pragma endregion



private: // メンバ変数

#pragma region ワールド座標系

	Transformation transform_;
	// 拡大率
	Vec3 m_scale;
	// 回転量
	Vec3 m_rotate;
	// 移動量
	Vec3 m_translate;
	// 画面サイズ
	Vec2 m_windowSize;

#pragma endregion

#pragma region カメラ座標系

	//
	Transformation cameraTransform_;

	// 拡大率
	Vec3 m_cameraScale;
	// 回転量
	Vec3 m_cameraRotate;
	// 移動量
	Vec3 m_cameraTranslate;
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

	Mat44 m_worldMatrix;
	Mat44 m_cameraMatrix;
	Mat44 m_viewMatrix;
	Mat44 m_projectionMatrix;
	Mat44 m_viewprojectionMatrix;
	Mat44 m_worldViewProjectionMatrix;
	Mat44 m_viewportMatrix;

#pragma endregion
	private:
	static MatrixCamera* instance;
};