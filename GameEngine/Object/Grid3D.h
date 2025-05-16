#pragma once
#include "GameEngine/Utility/Math/Math.h"
#include "GameEngine/Utility/Transform/WorldTransform.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Light/DirectionalLight.h"
#include "GameEngine/Object/Camera/CameraCommon.h"
#include "GameEngine/Resource/Model/ModelManager.h"

class CameraCommon;

// グリッド用頂点データ
struct VertexDataForGrid {
	Vector4 position;
	Color color;
};

class Grid3D
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gridsize">グリッドのセル数</param>
	/// <param name="gridspace">グリッドの間隔</param>
	Grid3D(int32_t gridsize,float gridspace);


	///	初期化
	void Init();
	/// 更新
	void Update();
	/// 描画
	void Draw();
	///	描画前処理
	void PreDraw();
	/// ImGuiの表示
	void DrawGUI();

private:	// -- private メンバ関数 -- //

	/// <summary>
	/// 3Dグリッド用のルートシグネチャを生成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// 3Dグリッド用のグラフィックスパイプラインを生成
	/// </summary>
	void CreateGraphicsPipeline();

	void CreateTransformation();
	void CreateVertexResource();
	void CreateIndexResource();
	void CreateMaterialResource();

public:

	// グリッドの数
	int32_t mGridsize;
	// グリッドの間隔
	float mGridspace;
	// 頂点データ
	std::vector<VertexDataForGrid> mGridVertices;
	// 頂点インデックス
	std::vector<uint32_t> mGridIndices;

	// 外部ポインタ
	Matrix4x4 viewM, wvpM;

	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mWvpResource = nullptr;
	// データを書き込む
	std::unique_ptr<TransformationMatrixForGrid3D> mWvpData = nullptr;
	std::unique_ptr<WorldTransform> mWorldTransform;

	// 頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexResource = nullptr;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};
	// 頂点データ
	VertexDataForGrid* mVertexData = nullptr;

	// Indexリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = nullptr;
	// Indexバッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};


private:

	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature = nullptr;

};

