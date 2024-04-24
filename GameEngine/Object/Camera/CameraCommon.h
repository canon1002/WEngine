#pragma once
#include "GameEngine/Math/Math.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Base/WinApp/WinAPI.h"

class CameraCommon
{
public: // -- public メンバ関数 -- //

	CameraCommon() = default;
	~CameraCommon() = default;

	void Initialize(WinAPI* winApp);
	
	virtual void Update();


public: // -- アクセッサ -- //

#pragma region Setter
	
	void SetRotate(Vector3 r) { worldTransform_->rotation = r; }
	void SetTranslate(Vector3 t) { worldTransform_->translation = t; }
	void SetTransform(Vector3 s, Vector3 r, Vector3 t) {
		worldTransform_->scale = s;
		worldTransform_->scale = r;
		worldTransform_->scale = t;
	}
	void SetFovY() {}
	void SetAspectRatio() {}
	void SetNearClip() {}
	void SetFarClip() {}
	void SetCamera() {}

#pragma endregion

#pragma region Getter

	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
	const Matrix4x4& GetViewProjectionMatrix() const { return viewprojectionMatrix_; }
	Vector3 GetRotate()const { return worldTransform_->rotation; }
	Vector3 GetTranslate()const { return worldTransform_->GetWorldPosition(); }

#pragma endregion

public: // -- public メンバ変数 -- //

	// WorldTransform
	WorldTransform* worldTransform_;

protected: // -- protected メンバ変数 -- //

	// winAppのポインタ
	WinAPI* winApp_ = nullptr;


	// ビュー行列
	Matrix4x4 viewMatrix_;
	// プロジェクション行列
	Matrix4x4 projectionMatrix_;
	// ビュープロジェクション行列
	Matrix4x4 viewprojectionMatrix_;

	// Near(近平面への距離)
	float nearClip_;
	// Far(遠平面への距離)
	float farClip_;
	// AspectRatio(アスペクト比)
	float aspectRatio_;
	// VerticalFOV(縦画角)
	float verticalFOV_;
};

