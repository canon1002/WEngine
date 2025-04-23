#pragma once
#include "GameEngine/Object/Model/Model.h"

// スカイボックス
class Skybox{

public:	// -- public メンバ関数 -- //

	// インスタンス取得
	static Skybox* GetInstance();

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

	void SetCameraPosition(CameraCommon camera) { mCameraData->worldPosition = camera.GetTranslate(); }

private: // -- private メンバ関数 -- //

	// コンストラクタ
	Skybox();
	// デストラクタ
	~Skybox();
	// コピーコンストラクタと演算子オーバーロードの禁止
	Skybox(const Skybox& obj) = delete;
	Skybox& operator=(const Skybox& obj) = delete;

	/// <summary>
	/// 座標関連の要素を作成する
	/// </summary>
	void CreateTransformation();

	/// <summary>
	/// 3Dモデル用のルートシグネチャを生成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// 3Dモデル用のグラフィックスパイプラインを生成
	/// </summary>
	void CreateGraphicsPipeline();

public: // -- public メンバ変数 -- //

	// インスタンス
	static Skybox* instance;
	// カメラ
	CameraCommon* mCamera = nullptr;

	// -- Object3Dの要素 -- //

	// 計算用の行列
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
	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexResource = nullptr;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};
	// 頂点リソースにデータを書き込む
	VertexData* mVertexData = nullptr;

	// Index用リソースデータ
	Microsoft::WRL::ComPtr<ID3D12Resource> mIndexResource = nullptr;
	//　インデックスはuint32_tとする
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView{};

	// マテリアル用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mMaterialResource = nullptr;

	// マテリアルデータ
	Material* mMaterialData = nullptr;

	// Light用のリソースデータを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mDirectionalLightResource = nullptr;
	// 平行光源　
	DirectionalLight* mDirectionalLightDate = nullptr;

	// 鏡面反射用のリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mCameraResource = nullptr;
	CameraForGPU* mCameraData = nullptr;

	// UVTransform用の変数
	UVTransform mUVTransform;

	// -- PSO やルートシグネチャ -- //

	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> mGraphicsPipelineState = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr <ID3D12RootSignature> mRootSignature = nullptr;
};

