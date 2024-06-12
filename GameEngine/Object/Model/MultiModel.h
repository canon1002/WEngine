#pragma once
#include "GameEngine/Math/Math.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Light/DirectionalLight.h"
#include "GameEngine/Object/Camera/CameraCommon.h"

// アニメーション
#include "GameEngine/Append/Animation/Skinning/Skinnig.h"

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
	void Initialize(DirectXCommon* dxCommon,CameraCommon* camera, const std::string& directrypath, const std::string& filename);
	void Initialize(const std::string& directrypath, const std::string& filename);
	void Update();
	void Draw();

	/// デバック情報描画(主にImGui関連)
	void DrawGUI(const std::string& label);

	void CreateVertexResource();
	void CreateIndexResource();
	void CreateMaterialResource();

	// カメラ座標を設定
	void SetCameraPosition(CameraCommon camera) { cameraData->worldPosition = camera.GetTranslate(); }

public:

	// ModelCommonのポインタ
	//ModelCommon* modelCommon_ = nullptr;
	// DirectXのポインタ
	DirectXCommon* mDxCommon = nullptr;
	CameraCommon* camera_ = nullptr;

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
	Skinnig* skinning_ = nullptr;

};

