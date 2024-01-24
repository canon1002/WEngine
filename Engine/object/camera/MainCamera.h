#pragma once
#include"../math/Math.h"
#include"../worldTransform/WorldTransform.h"

class MainCamera
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
	void Update();

	/// Setter
	void SetRotate() {}
	void SetTranslate(){}
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
	// 拡大・回転・平行移動
	Transformation transform_;
	// ワールド行列
	Mat44 worldMatrix_;
	// ビュー行列
	Mat44 viewMatrix_;
	// プロジェクション行列
	Mat44 projectionMatrix_;
	// ビュープロジェクション行列
	Mat44 viewprojectionMatrix_;

	// Near(近平面への距離)
	float nearClip_;
	// Far(遠平面への距離)
	float farClip_;
	// AspectRatio(アスペクト比)
	float aspectRatio_;
	// VerticalFOV(縦画角)
	float verticalFOV_;
};

