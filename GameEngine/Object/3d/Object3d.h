#pragma once
#include "GameEngine/Object/Base/ObjectBase.h"
#include "GameEngine/Utility/Math/Math.h"
#include "GameEngine/Utility/Transform/WorldTransform.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Light/DirectionalLight.h"
#include "GameEngine/Object/Camera/CameraCommon.h"
#include "GameEngine/Component/Animation/Skinning/Skinnig.h"
#include "GameEngine/Component/Collider/Collider.h"

// 前方宣言
class Model; // モデルクラス
class NewMaterial; // マテリアルクラス
class CameraCommon; // カメラ基底クラス
class ModelManager; // モデルマネージャークラス

class Object3d : public ObjectBase
{
public: // -- 公開 メンバ関数 -- //

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

	/// <summary>
	/// 座標リソース生成
	/// </summary>
	void CreateTransformation();



	void SetScale(Vector3 scale) { mWorldTransform->scale = scale; }
	void SetRotate(Vector3 rotate) { mWorldTransform->rotation = rotate; }
	void SetTranslate(Vector3 translate) { mWorldTransform->translation = translate; }
	const WorldTransform* GetWorldTransform()const { return mWorldTransform.get(); }

	// モデル設定
	void SetModel(const std::string& filePath);
	// モデル設定(フルパス)
	void SetModelFullPath(const std::string& directryPath, const std::string& filePath);
	// モデル取得
	Model* GetModel()const { return mModel; }

	// マテリアル生成処理
	MaterialExt* CreateMaterial()const;

public: // -- 公開 メンバ変数 -- //

	// モデル
	Model* mModel = nullptr;
	// マテリアル
	MaterialExt* mMaterial;

	// スキニング アニメーション
	std::unique_ptr<Skinning> mSkinning = nullptr;
	// コライダー
	std::unique_ptr<Collider> mCollider = nullptr;

	// Transformation用Resource
	Microsoft::WRL::ComPtr<ID3D12Resource> mWvpResource = nullptr;
	TransformationMatrix* mWvpData;
};

