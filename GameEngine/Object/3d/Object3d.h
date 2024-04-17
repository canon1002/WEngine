#pragma once
#include "GameEngine/Math/Math.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Light/DirectionalLight.h"
#include "GameEngine/Object/Camera/CameraCommon.h"

class Object3dCommon;
class Model;
class CameraCommon;
class ModelManager;

class Object3d
{

public: // メンバ関数

	Object3d();
	~Object3d();

	///	初期化
	void Init(DirectXCommon* dxCommon, ModelManager* modelManager);
	/// 更新
	void Update(const CameraCommon& camera);
	/// 描画
	void Draw();

	void CreateTransformationRsource();
	void SetModel(const std::string& filepath);
	void SetScale(Vec3 scale) { worldTransform_.scale = scale; }
	void SetRotate(Vec3 rotate) { worldTransform_.rotation = rotate; }
	void SetTranslate(Vec3 translate) { worldTransform_.translation = translate; }
	void SetWorldTransform(WorldTransform w) { worldTransform_ = w; }
	const WorldTransform& GetWorldTransform()const  { return worldTransform_; }

	Model* GetModel() { return model_; }

private: // メンバ変数

	// 外部ポインタ
	DirectXCommon* dxCommon_ = nullptr;
	Object3dCommon* object3dCommon_ = nullptr;
	Model* model_ = nullptr;
	ModelManager* modelManager_;

	Mat44 cameraM, viewM, projectM, pespectiveM, wvpM;
	
	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = nullptr;
	// データを書き込む
	TransformationMatrix* wvpData = nullptr;
	WorldTransform worldTransform_;


};

