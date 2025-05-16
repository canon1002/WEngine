#pragma once
#include "GameEngine/Utility/Math/Math.h"
#include "GameEngine/Utility/Transform/WorldTransform.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Light/DirectionalLight.h"
#include "GameEngine/Object/Camera/CameraCommon.h"

// アニメーション
#include "GameEngine/Component/Animation/Skinning/Skinnig.h"


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

	~Model();
	void Init(const std::string& directrypath, const std::string& filename);
	void Update();
	void Draw();

	/// デバック情報描画(主にImGui関連)
	void DrawGUI(const std::string& label);

	void CreateVertexResource();
	void CreateIndexResource();
	void CreateMaterialResource();

	// カメラ座標を設定
	inline void SetCameraPosition(CameraCommon camera) { mCameraData->worldPosition = camera.GetTranslate(); }
	inline void SetCubeTexture(const int32_t& textureHandle) { mTextureHandleCubeMap = textureHandle; }


public: // -- 公開 メンバ変数 -- //

	// モデルデータ
	ModelData mModelData;
	// テクスチャハンドル
	int32_t mTextureHandle;
	int32_t mTextureHandleCubeMap;

	// -- Vertex -- //
	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexResource = nullptr;
	VertexData* mVertexData = nullptr;
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};


	// -- Material -- //
	Microsoft::WRL::ComPtr<ID3D12Resource> mMaterialResource = nullptr;
	Material* mMaterialData = nullptr;


	// -- Transfomation -- //
	Microsoft::WRL::ComPtr<ID3D12Resource> mWvpResource = nullptr;
	TransformationMatrix* mWvpData = nullptr;


	// -- Index -- //
	Microsoft::WRL::ComPtr<ID3D12Resource> mIndexResource = nullptr;
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView{};


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

