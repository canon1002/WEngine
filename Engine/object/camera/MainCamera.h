#pragma once
#include "Engine/Math/Math.h"
#include "Engine/Base/DirectXCommon.h"
#include "Engine/Object/Texture/Resource.h"
#include "Engine/Object/WorldTransform/WorldTransform.h"
#include "Engine/Object/Camera/CameraCommon.h"

class MainCamera:
	public CameraCommon
{
private:

	static MainCamera* instance;
	MainCamera();
	~MainCamera() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	MainCamera(const MainCamera& obj) = delete;
	MainCamera& operator=(const MainCamera& obj) = delete;

public:

	static MainCamera* GetInstance();
	void Finalize();
	void Initialize(Transformation transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} });
	//void Initialize();
	void Update()override;

	/// Setter
	void SetRotate(Vec3 r) { transform_.rotate = r; }
	void SetTranslate(Vec3 t){ transform_.translate = t; }
	void SetFovY(){}
	void SetAspectRatio(){}
	void SetNearClip(){}
	void SetFarClip(){}
	void SetCamera(){}

	///getter

	const Mat44& GetWorldMatrix() const { return worldMatrix_; }
	const Mat44& GetViewMatrix() const { return viewMatrix_; }
	const Mat44& GetProjectionMatrix() const { return projectionMatrix_; }
	const Mat44& GetViewProjectionMatrix() const { return viewprojectionMatrix_;}
	const Transformation& GetTransform()const { return transform_; }
	const Vec3& GetRotate()const { return transform_.rotate; }
	const Vec3& GetTranslate()const { return transform_.translate; }


private:
	
};

