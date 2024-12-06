#pragma once
#include "GameEngine/Object/Base/ObjectBase.h"

#include "GameEngine/Math/Math.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Light/DirectionalLight.h"
#include "GameEngine/Object/Camera/CameraCommon.h"
#include "GameEngine/Append/Animation/Skinning/Skinnig.h"

#include "GameEngine/Append/Collider/Collider.h"

class Object3dCommon;
class Model;
class CameraCommon;
class ModelManager;

class Object3d : public ObjectBase 
{

public: // メンバ関数

	Object3d() = default;
	Object3d(const std::string objname);
	~Object3d();

	///	初期化
	void Init(std::string name);
	/// 更新
	void Update();
	/// 描画
	void Draw();
	void DrawGUI();
	void DrawGuiTree();

	void CreateTransformation();
	void SetModel(const std::string& filepath);
	void SetScale(Vector3 scale) { mWorldTransform->scale = scale; }
	void SetRotate(Vector3 rotate) { mWorldTransform->rotation = rotate; }
	void SetTranslate(Vector3 translate) { mWorldTransform->translation = translate; }
	void SetWorldTransform(WorldTransform* w) { mWorldTransform = w; }
	const WorldTransform* GetWorldTransform()const  { return mWorldTransform; }

	Model* GetModel() { return mModel; }
	
public: // メンバ変数

	// 外部ポインタ
	DirectXCommon* mDxCommon = nullptr;
	Object3dCommon* mObject3dCommon = nullptr;
	Model* mModel = nullptr;
	ModelManager* mModelManager;
	Matrix4x4 cameraM, viewM, projectM, pespectiveM, wvpM;

	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mWvpResource = nullptr;
	// データを書き込む
	TransformationMatrix* mWvpData = nullptr;
	//WorldTransform* mWorldTransform;

	// マテリアル
	Material material;

	// スキニング アニメーション
	Skinning* mSkinning = nullptr;
	// スケルトン
	Skeleton mSkeleton;

	// コライダー
	Collider* mCollider;


};

