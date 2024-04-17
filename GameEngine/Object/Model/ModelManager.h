#pragma once
#include <unordered_map>
#include <sstream>
#include "Model.h"

class ModelManager{
public: // -- public メンバ関数 -- //

	ModelManager() = default;
	~ModelManager() = default;

	void Finalize();
	void Initialize(DirectXCommon* dxCommon, CameraCommon* camera);
	void LoadModel(const std::string& filepath);
	Model* FindModel(const std::string filepath);

	void PostDraw();

private: // -- private メンバ関数 -- //

	/// <summary>
	/// 3Dモデル用のルートシグネチャを生成
	/// </summary>
	void CreateRootSignature();
	
	/// <summary>
	/// 3Dモデル用のグラフィックスパイプラインを生成
	/// </summary>
	void CreateGraphicsPipeline();

	// コピーコンストラクタと演算子オーバーロードの禁止
	ModelManager(const ModelManager& obj) = delete;
	ModelManager& operator=(const ModelManager& obj) = delete;


private: // -- private メンバ変数 -- //

	// ポインタ
	DirectXCommon* dxCommon_ = nullptr;
	CameraCommon* camera_ = nullptr;

	// モデルデータ
	std::unordered_map<std::string, std::unique_ptr<Model>> models;

	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature = nullptr;

};

