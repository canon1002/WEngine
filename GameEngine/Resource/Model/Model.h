#pragma once
#include "GameEngine/Utility/Math/Math.h"
#include "GameEngine/Utility/Transform/WorldTransform.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Light/DirectionalLight.h"
#include "GameEngine/Object/Camera/CameraCommon.h"
#include "GameEngine/Component/Animation/Skinning/Skinnig.h"

// メッシュデータ
struct MeshData {
	// 頂点
	std::vector<VertexData> vertices;
	// インデックス
	std::vector<uint32_t> indices;
	// マテリアル
	MaterialData material;
	// スキンクラスター
	std::map<std::string, JointWeightData> skinClusterData;
};

// モデルデータ
struct ModelData {
	MeshData mesh;
	Node rootNode;
};

// マルチメッシュモデルデータ
struct MultiModelData {
	std::vector<MeshData> meshes;
	Node rootNode;
};

class Model
{
public: // -- 公開 メンバ関数 -- //

	// コンストラクタ
	Model() = default;
	// デストラクタ
	~Model();
	// 初期化
	void Init(const std::string& directrypath, const std::string& filename);
	// 更新
	void Update();
	// 描画
	void Draw();
	// ImGui描画
	void DrawGUI(const std::string& label);


	// カメラ座標を設定
	void SetCameraPosition(CameraCommon camera) {
		mCameraData->worldPosition = camera.GetTranslate();
	}

	// キューブマップのテクスチャハンドルを設定
	void SetCubeTexture(const int32_t& textureHandle) {
		mTextureHandleCubeMap = textureHandle;
	}
		

private: // -- 非公開 メンバ関数 -- //

	// 頂点リソース生成
	void CreateVertexResource();
	// インデックスリソース生成
	void CreateIndexResource();
	// マテリアルリソース生成
	void CreateMaterialResource();

public: // -- 公開 メンバ変数 -- //

	// モデルデータ
	MultiModelData mModelData;

	// テクスチャハンドル
	std::vector<int32_t> mTextureHandles;
	int32_t mTextureHandleCubeMap;

	// -- Vertex -- //

	// 頂点リソース
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> mVertexResources;
	// 頂点バッファビュー
	std::vector<D3D12_VERTEX_BUFFER_VIEW> mVertexBufferViews;

	// -- Index -- //

	// インデックスリソース
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> mIndexResources;
	// インデックスバッファビュー
	std::vector<D3D12_INDEX_BUFFER_VIEW> mIndexBufferViews;

	// -- Material -- //

	// マテリアルリソース
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> mMaterialResources;
	// マテリアルデータ
	std::vector<Material*> mMaterialDatas;


	// -- Light -- //

	Microsoft::WRL::ComPtr<ID3D12Resource> mDirectionalLightResource = nullptr;
	DirectionalLight* mDirectionalLightData = nullptr; // 平行光源　
	bool mIsLighting = true;

	// -- 鏡面反射 -- //

	// -- カメラ -- // 
	Microsoft::WRL::ComPtr<ID3D12Resource> mCameraResource = nullptr;
	CameraForGPU* mCameraData = nullptr;

	// UVTransform用の変数
	UVTransform mUvTransform;


private: // --  非公開 メンバ変数 -- //

};

