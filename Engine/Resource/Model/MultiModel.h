#pragma once
#include "Math/Math.h"
#include "Component/Transform/WorldTransform.h"
#include "Resource/Texture/Texture.h"
#include "Resource/Base/Resource.h"
#include "Object/Light/DirectionalLight.h"
#include "Object/Camera/Camera.h"

// アニメーション
#include "Component/Animation/Skinning/Skinnig.h"

// メッシュ
struct Mesh {
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	UINT materialIndex;
	std::map<std::string, JointWeightData>skinClusterData;
};

// モデルデータ(マルチメッシュ対応版)
struct MultiModelData {
	
	Node rootNode;
	std::vector<Mesh> meshes;
	std::vector<MultiMaterial> materials;
	
};

class MultiModel
{
public:

	~MultiModel();
	void Initialize(DirectXCommon* dxCommon,Camera* camera, const std::string& directrypath, const std::string& filename);
	void Initialize(const std::string& directrypath, const std::string& filename);
	void Update();
	void Draw();

	/// デバック情報描画(主にImGui関連)
	void DrawGUI(const std::string& label);

	void CreateVertexResource();
	void CreateIndexResource();
	void CreateMaterialResource();

	// カメラ座標を設定
	void SetCameraPosition(Camera camera) { cameraData->worldPosition = camera.GetWorld().translation; }

public:

	// ModelCommonのポインタ
	//ModelCommon* modelCommon_ = nullptr;
	// DirectXのポインタ
	DirectXCommon* mDxCommon = nullptr;
	Camera* mCamera = nullptr;

	// モデルデータ
	MultiModelData modelData;

	
public:

	// Light用のリソースデータを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = nullptr;
	// 平行光源　
	DirectionalLight* directionalLightDate = nullptr;
	// 平行光線の適用をImGuiで操作する用のフラグ
	bool isLighting_ = true;

	// 鏡面反射用のリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> CameraResource = nullptr;
	CameraForGPU* cameraData = nullptr;

	// UVTransform用の変数
	UVTransform uvTransform_;

	// -- Animation 関連 -- //

	// スキニング アニメーション
	Skinnig* mSkinning = nullptr;

};

