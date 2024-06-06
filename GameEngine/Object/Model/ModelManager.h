#pragma once
#include <unordered_map>
#include <map>
#include <sstream>
#include "Model.h"

class ModelManager{
public: // -- public メンバ関数 -- //

	ModelManager() = default;
	~ModelManager() = default;

	static ModelManager* GetInstance();

	void Finalize();
	void Initialize(DirectXCommon* dxCommon, CameraCommon* camera);
	void LoadModel(const std::string& directoryPath,const std::string& filepath);
	Model* FindModel(const std::string filepath);

	static std::shared_ptr<Model> Create(const std::string& filepath, const std::string filename);

	/// <summary>
	///  描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// Skinning用描画前処理
	/// </summary>
	void PreDrawForSkinning();
	
	/// <summary>
	///  描画処理
	/// </summary>
	void Draw();


private: // -- private メンバ関数 -- //

	/// <summary>
	/// 3Dモデル用のルートシグネチャを生成
	/// </summary>
	void CreateRootSignature();
	
	/// <summary>
	/// 3Dモデル用のグラフィックスパイプラインを生成
	/// </summary>
	void CreateGraphicsPipeline();

	/// <summary>
	/// Skinning用のルートシグネチャを生成
	/// </summary>
	void CreateRootSignatureForSkinning();

	/// <summary>
	/// Skinning用のグラフィックスパイプラインを生成
	/// </summary>
	void CreateGraphicsPipelineForSkinning();

	// コピーコンストラクタと演算子オーバーロードの禁止
	ModelManager(const ModelManager& obj) = delete;
	ModelManager& operator=(const ModelManager& obj) = delete;

private: // -- private メンバ変数 -- //

	//
	static std::map<std::string, std::shared_ptr<Model>> sModels_;

	// ポインタ
	DirectXCommon* mDxCommon = nullptr;
	CameraCommon* camera_ = nullptr;

	// モデルデータ
	std::unordered_map<std::string, std::unique_ptr<Model>> models;

	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineStateForSkinning = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignatureForSkinning = nullptr;

	// インスタンス
	static ModelManager* instance;

};

