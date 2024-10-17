#pragma once
#include "Math/Math.h"
#include "Resource/Base/Resource.h"
#include "Resource/Texture/Texture.h"
#include "Object/Light/DirectionalLight.h"
#include "Object/Camera/Camera.h"

// アニメーション
#include "Component/Animation/Skinning/Skinnig.h"


// モデルデータ
struct ModelData {
	std::map<std::string, JointWeightData>skinClusterData;
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	MaterialData material;
	Node rootNode;
};

class Model
{
public:

	Model() = default;
	~Model();
	static Model* Create(const std::string& directrypath, const std::string& filename);
	void Init();
	void Draw(Camera camera);


	/// デバック情報描画(主にImGui関連)
	void DrawGUI(const std::string& label);

	void CreateVertexResource();
	void CreateIndexResource();
	void CreateMaterialResource();
	inline void SetCubeTexture(const int32_t& textureHandle) { mTextureHandleCubeMap = textureHandle; }
public:

	// モデルデータ
	ModelData modelData;
	// テクスチャハンドル
	int32_t mTextureHandle;
	int32_t mTextureHandleCubeMap;

	// 頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexResource = nullptr;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};
	// 頂点データ
	VertexData* mVertexData = nullptr;
	
	// Indexリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = nullptr;
	// Indexバッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	

	// マテリアル用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;

public:

	// マテリアルデータ
	Material* materialData_ = nullptr;

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

};

