#pragma once
#include "Math/Math.h"
#include "Component/Transform/WorldTransform.h"
#include "Resource/Base/Resource.h"
#include "Object/Light/DirectionalLight.h"
#include "Object/Camera/Camera.h"
#include "Component/Animation/Skinning/Skinnig.h"

#include "Component/Collider/Collider.h"

class Object3dCommon;
class Model;
class Camera;
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
	void DrawGuiTree();

	void CreateTransformation();
	void SetModel(const std::string& filepath);
	void SetScale(Vector3 scale) { mWorldTransform.scale = scale; }
	void SetRotate(Vector3 rotate) { mWorldTransform.rotation = rotate; }
	void SetTranslate(Vector3 translate) { mWorldTransform.translation = translate; }
	void SetWorldTransform(WorldTransform* w) { mWorldTransform = w; }
	const WorldTransform* GetWorldTransform()const  { return mWorldTransform; }

	Model* GetModel() { return mModel; }

public: // メンバ変数

	// オブジェクトの名称
	std::string mObjname;

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
	WorldTransform* mWorldTransform;

	// マテリアル
	Material material;

	// スキニング アニメーション
	Skinnig* mSkinning = nullptr;
	// スケルトン
	Skeleton mSkeleton;

	// コライダー
	Collider* mCollider;


};

