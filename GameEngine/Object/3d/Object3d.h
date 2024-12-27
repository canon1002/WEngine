#pragma once
#include "GameEngine/Object/Base/ObjectBase.h"

#include "GameEngine/Math/Math.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Light/DirectionalLight.h"
#include "GameEngine/Object/Camera/CameraCommon.h"
#include "GameEngine/Append/Animation/Skinning/Skinnig.h"

#include "GameEngine/Append/Collider/Collider.h"

// 前方宣言
class Model; // モデルクラス
class CameraCommon; // カメラ基底クラス
class ModelManager; // モデルマネージャークラス

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
	void SetModelFullPath(const std::string& directryPath, const std::string& filePath);
	void SetScale(Vector3 scale) { mWorldTransform->scale = scale; }
	void SetRotate(Vector3 rotate) { mWorldTransform->rotation = rotate; }
	void SetTranslate(Vector3 translate) { mWorldTransform->translation = translate; }
	void SetWorldTransform(std::shared_ptr<WorldTransform> world) { mWorldTransform = world; }
	const std::shared_ptr<WorldTransform> GetWorldTransform()const  { return mWorldTransform; }

	Model* GetModel() { return mModel.get(); }
	std::shared_ptr<Model>  GetModelPtr()const { return mModel; }
	
public: // メンバ変数

	// モデル
	std::shared_ptr<Model> mModel = nullptr;
	
	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mWvpResource = nullptr;
	// データを書き込む
	std::shared_ptr<TransformationMatrix> mWvpData = nullptr;

	// マテリアル
	Material material;

	// スキニング アニメーション
	std::unique_ptr<Skinning> mSkinning = nullptr;
	
	// コライダー
	Collider* mCollider;


};

