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
	// テクスチャ
	std::string textureFilePath;
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

/// <summary>
/// モデルクラス
/// </summary>
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
	//void Draw();
	//void DrawSkinning(Skinning* skinning, const bool& isActive = false);
	// ImGui描画
	void DrawGUI(const std::string& label);

	void Draw(const MaterialExt& materialExt);
	void DrawSkinning(const MaterialExt& materialExt, Skinning* skinning, const bool& isActive = false);


	// カメラ座標を設定
	void SetCameraPosition(CameraCommon camera) {
		mCameraData->worldPosition = camera.GetTranslate();
	}

	// キューブマップのテクスチャハンドルを設定
	void SetCubeTexture(const int32_t& textureHandle) {
		mTextureHandleCubeMap = textureHandle;
	}


	// -- アクセッサ -- //

	/// <summary>
	/// 頂点バッファビューを取得
	/// <para> index未入力の場合、最初のVBVを返す </para>
	/// </summary>
	/// <param name="index"> バッファ番号 </param>
	/// <returns></returns>
	D3D12_VERTEX_BUFFER_VIEW GetVBV(const int32_t& index = 0) {
		return mVertexBufferViews[index];
	}

	/// <summary>
	/// インデックスバッファビューを取得
	/// <para> index未入力の場合、最初のIBVを返す </para>
	/// </summary>
	/// <param name="index"> バッファ番号 </param>
	/// <returns></returns>
	D3D12_INDEX_BUFFER_VIEW GetIBV(const int32_t& index = 0) {
		return mIndexBufferViews[index];
	}

	// マテリアル生成処理
	std::unique_ptr<MaterialExt> CreateMaterial()const;


private: // -- 非公開 メンバ関数 -- //

	// 頂点リソース生成
	void CreateVertexResource();
	// インデックスリソース生成
	void CreateIndexResource();

public: // -- 公開 メンバ変数 -- //

	// モデルデータ
	MultiModelData mModelData;
	// キューブマップテクスチャハンドル
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

