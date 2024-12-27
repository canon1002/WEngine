#pragma once
#include "GameEngine/Math/Math.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Base/WinApp/WinAPI.h"

class CameraCommon
{
public: // -- public メンバ関数 -- //

	CameraCommon() = default;
	~CameraCommon() = default;

	virtual void Initialize();
	
	virtual void Update();


public: // -- アクセッサ -- //

#pragma region Setter
	
	void SetRotate(Vector3 r) { mWorldTransform->rotation = r; }
	void SetTranslate(Vector3 t) { mWorldTransform->translation = t; }
	void SetTransform(Vector3 s, Vector3 r, Vector3 t) {
		mWorldTransform->scale = s;
		mWorldTransform->scale = r;
		mWorldTransform->scale = t;
	}
	void SetFovY() {}
	void SetAspectRatio() {}
	void SetNearClip() {}
	void SetFarClip() {}
	void SetCamera() {}
	
	float GetFarClip()const { return farClip_; }
	float GetNearClip()const { return nearClip_; }

#pragma endregion

#pragma region Getter

	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
	const Matrix4x4& GetViewProjectionMatrix() const { return viewprojectionMatrix_; }
	Vector3 GetRotate()const { return mWorldTransform->rotation; }
	Vector3 GetTranslate()const { return mWorldTransform->GetWorldPosition(); }

	Vector2 GetWindowSize()const { return Vector2((float)WinApp::GetInstance()->kClientWidth, (float)WinApp::GetInstance()->kClientHeight); }

#pragma endregion

public: // -- public メンバ変数 -- //

	// WorldTransform
	std::shared_ptr<WorldTransform> mWorldTransform;

protected: // -- protected メンバ変数 -- //

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

