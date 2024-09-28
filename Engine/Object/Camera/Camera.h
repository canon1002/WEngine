#pragma once
#include "Object/Base/ObjectBase.h"
#include "Component/Transform/WorldTransform.h"
#include "Math/Math.h"


/// <summary>
/// カメラクラス
/// </summary>
class Camera : public ObjectBase
{
public: // -- 公開 メンバ関数 -- //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Camera();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Camera();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="objectName">オブジェクト名</param>
	virtual void Init(std::string objectName) override;

	/// <summary>
	///  更新処理
	/// </summary>
	virtual void Update() override;

	/// <summary>
	/// デバッグ描画
	/// </summary>
	virtual void Debug() override;

	Matrix4x4 GetViewMatrix() { return mViewMatrix; }
	Matrix4x4 GetProjectionMatrix() { return mProjectionMatrix_; }
	Matrix4x4 GetViewProjectionMatrix() { return mViewMatrix; }

protected: // -- 限定公開 メンバ変数 -- //

	
	// ビュー行列
	Matrix4x4 mViewMatrix;
	// プロジェクション行列
	Matrix4x4 mProjectionMatrix_;
	// ビュープロジェクション行列
	Matrix4x4 mViewprojectionMatrix_;

	// Near(近平面への距離)
	float nearClip_;
	// Far(遠平面への距離)
	float farClip_;
	// AspectRatio(アスペクト比)
	float aspectRatio_;
	// VerticalFOV(縦画角)
	float verticalFOV_;
	
};

