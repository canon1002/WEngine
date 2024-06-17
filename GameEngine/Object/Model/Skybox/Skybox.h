#pragma once
#include "GameEngine/Object/Model/Model.h"

class Skybox{

public:	// -- public メンバ関数 -- //

	~Skybox();
	void Init(const std::string& directrypath, const std::string& filename);
	void Update();
	void Draw();

	/// <summary>
	///  描画前処理
	/// </summary>
	void PreDraw();

	/// デバック情報描画(主にImGui関連)
	void DrawGUI(const std::string& label);

	void CreateVertexResource();
	void CreateMaterialResource();

	void SetCameraPosition(CameraCommon camera) { cameraData->worldPosition = camera.GetTranslate(); }

private: // -- private メンバ関数 -- //

	/// <summary>
	/// 座標関連の要素を作成する
	/// </summary>
	void CreateTransformationRsource();

	/// <summary>
	/// 3Dモデル用のルートシグネチャを生成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// 3Dモデル用のグラフィックスパイプラインを生成
	/// </summary>
	void CreateGraphicsPipeline();

public: // -- public メンバ変数 -- //

	// ModelCommonのポインタ
	//ModelCommon* modelCommon_ = nullptr;
	// DirectXのポインタ
	DirectXCommon* mDxCommon = nullptr;
	CameraCommon* mCamera = nullptr;

	// -- Object3Dの要素 -- //

	Matrix4x4 cameraM, viewM, projectM, pespectiveM, wvpM;
	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mWvpResource = nullptr;
	// データを書き込む
	TransformationMatrix* mWvpData = nullptr;
	WorldTransform* mWorldTransform;


	// -- Model 部分 -- //

	
	// ddsのテクスチャハンドル
	int32_t mTextureHandle;

	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;

	// Index用リソースデータ
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = nullptr;
	//　インデックスはuint32_tとする
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	// マテリアル用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;

	// マテリアルデータ
	Material* materialData_ = nullptr;

	// Light用のリソースデータを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = nullptr;
	// 平行光源　
	DirectionalLight* directionalLightDate = nullptr;

	// 鏡面反射用のリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> CameraResource = nullptr;
	CameraForGPU* cameraData = nullptr;

	// UVTransform用の変数
	UVTransform uvTransform_;

	// -- PSO やルートシグネチャ -- //

	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature = nullptr;
};

