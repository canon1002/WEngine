#pragma once

#include "GameEngine/Math/Math.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Light/DirectionalLight.h"
#include "GameEngine/Object/Camera/CameraCommon.h"

#include <DirectXMath.h>

class Model;
class CameraCommon;

// シャドウマップ用
struct SceneMatrix {
	DirectX::XMMATRIX view;		// ビュー
	DirectX::XMMATRIX proj;		// プロジェクション
	DirectX::XMMATRIX shadow;	// 影
	DirectX::XMFLOAT3 eye;		// 視点
};


class ShadowObject{

public:	// -- 公開 メンバ関数 -- // 

	ShadowObject();
	ShadowObject(const std::string objname);
	~ShadowObject();

	///	初期化
	void Init(std::string name);
	/// 更新
	void Update();
	/// 描画
	void Draw();
	void DrawGUI();
	
	void CreateTransformation();
	void CreateSceneResource();
	void SetModel(const std::string& filepath);
	void SetScale(Vector3 scale) { mWorldTransform->scale = scale; }
	void SetRotate(Vector3 rotate) { mWorldTransform->rotation = rotate; }
	void SetTranslate(Vector3 translate) { mWorldTransform->translation = translate; }
	void SetWorldTransform(WorldTransform* w) { mWorldTransform = w; }
	const WorldTransform* GetWorldTransform()const { return mWorldTransform; }

	
public:	// --公開 メンバ変数 -- // 

	// オブジェクトの名称
	std::string mObjname;

	// 外部ポインタ
	
	
	Model* mModel = nullptr;
	
	Matrix4x4 cameraM, viewM, projectM, pespectiveM, wvpM;

	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mWvpResource = nullptr;
	// データを書き込む
	TransformationMatrix* mWvpData = nullptr;
	WorldTransform* mWorldTransform;

	// シャドウ用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mSceneResource = nullptr;
	

	// マテリアル
	Material material;

	// 平行ライトの向き
	DirectX::XMFLOAT3 mParallelLightVector;

	// 影行列用の構造体
	SceneMatrix* mMappedScene;
	
};

