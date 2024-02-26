#pragma once
#include "Math.h"
#include "../worldTransform/WorldTransform.h"
#include "../../resources/Section/Resource.h"
#include "../../object/light/DirectionalLight.h"
#include "../camera/CameraCommon.h"

class Object3dCommon;
class Model;
class MainCamera;

class Object3d
{

public: // メンバ関数

	Object3d();
	~Object3d();

	///	初期化
	void Init();
	/// 更新
	void Update();
	/// 描画
	void Draw();

	void CreateTransformationRsource();
	void SetModel(const std::string& filepath);
	void SetScale(Vec3 scale) { worldTransform_.scale = scale; }
	void SetRotate(Vec3 rotate) { worldTransform_.rotate = rotate; }
	void SetTranslate(Vec3 translate) { worldTransform_.translate = translate; }
	void SetWorldTransform(WorldTransform w) { worldTransform_ = w; }
	void SetCamera(CameraCommon* camera) { camera_ = camera; }
	const WorldTransform& GetWorldTransform()const  { return worldTransform_; }
	void UpdateWorldMat() {
		worldTransform_.worldM = MakeAffineMatrix(worldTransform_.scale,
			worldTransform_.rotate, worldTransform_.translate);
	}

	Model* GetModel() { return model_; }

private: // メンバ変数

	// 外部ポインタ
	CameraCommon* camera_ = nullptr;
	DirectXCommon* dx_ = nullptr;
	Object3dCommon* object3dCommon_ = nullptr;
	Model* model_ = nullptr;
	
	Mat44 cameraM, viewM, projectM, pespectiveM, wvpM;
	
	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = nullptr;
	// データを書き込む
	TransformationMatrix* wvpData = nullptr;
	WorldTransform worldTransform_;


};

