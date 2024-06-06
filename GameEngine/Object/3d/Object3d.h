#pragma once
#include "GameEngine/Math/Math.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Light/DirectionalLight.h"
#include "GameEngine/Object/Camera/CameraCommon.h"
#include "GameEngine/Append/Animation/Skinning/Skinnig.h"

class Object3dCommon;
class Model;
class CameraCommon;
class ModelManager;

class Object3d
{

public: // メンバ関数

	Object3d();
	Object3d(const std::string objname);
	~Object3d();

	///	初期化
	void Init(std::string name);
	/// 更新
	void Update();
	/// 描画
	void Draw();
	void DrawGUI();

	void CreateTransformationRsource();
	void SetModel(const std::string& filepath);
	void SetScale(Vector3 scale) { worldTransform_->scale = scale; }
	void SetRotate(Vector3 rotate) { worldTransform_->rotation = rotate; }
	void SetTranslate(Vector3 translate) { worldTransform_->translation = translate; }
	void SetWorldTransform(WorldTransform* w) { worldTransform_ = w; }
	const WorldTransform* GetWorldTransform()const  { return worldTransform_; }

	Model* GetModel() { return model_; }

public: // メンバ変数

	// オブジェクトの名称
	std::string objname_;

	// 外部ポインタ
	DirectXCommon* mDxCommon = nullptr;
	Object3dCommon* object3dCommon_ = nullptr;
	Model* model_ = nullptr;
	ModelManager* modelManager_;
	Matrix4x4 cameraM, viewM, projectM, pespectiveM, wvpM;

	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = nullptr;
	// データを書き込む
	TransformationMatrix* wvpData = nullptr;
	WorldTransform* worldTransform_;

	// -- Animation 関連 -- //

	// アニメーションデータを保有しているか
	bool isHavingAnimation_ = false;

	// アニメーション再生中の時刻
	float animationTime_ = 0.0f;

	// スケルトン 仮置き
	Skeleton skeleton_;
	// スキンクラスター 仮置き
	SkinCluster skinCluster_;
};

