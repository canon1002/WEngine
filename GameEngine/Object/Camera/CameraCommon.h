#pragma once
#include "GameEngine/Utility/Math/Math.h"
#include "GameEngine/Utility/Transform/WorldTransform.h"
#include "GameEngine/Base/WinApp/WinAPI.h"

class CameraCommon
{
public: // -- public メンバ関数 -- //

	CameraCommon() = default;
	~CameraCommon() = default;

	virtual void Init();
	
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
	
	float GetFarClip()const { return mFarClip; }
	float GetNearClip()const { return mNearClip; }

#pragma endregion

#pragma region Getter

	const Matrix4x4& GetViewMatrix() const { return mViewMat; }
	const Matrix4x4& GetProjectionMatrix() const { return mProjectionMat; }
	const Matrix4x4& GetViewProjectionMatrix() const { return mViewProjectionMat; }
	Vector3 GetRotate()const { return mWorldTransform->rotation; }
	Vector3 GetTranslate()const { return mWorldTransform->GetWorldPosition(); }

	Vector2 GetWindowSize()const { return Vector2((float)WinApp::GetInstance()->kClientWidth, (float)WinApp::GetInstance()->kClientHeight); }

#pragma endregion

public: // -- public メンバ変数 -- //

	// WorldTransform
	std::unique_ptr<WorldTransform> mWorldTransform;

protected: // -- protected メンバ変数 -- //

	// ビュー行列
	Matrix4x4 mViewMat;
	// プロジェクション行列
	Matrix4x4 mProjectionMat;
	// ビュープロジェクション行列
	Matrix4x4 mViewProjectionMat;

	// Near(近平面への距離)
	float mNearClip;
	// Far(遠平面への距離)
	float mFarClip;
	// AspectRatio(アスペクト比)
	float mAspectRatio;
	// VerticalFOV(縦画角)
	float mVerticalFOV;
};

