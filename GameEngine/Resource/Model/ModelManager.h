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
	void Init();
	//void Initialize(DirectXCommon* dxCommon, Camera* camera);
	void LoadModel(const std::string& directoryPath,const std::string& filepath);
	void LoadMultiModel(const std::string& directoryPath,const std::string& filepath);
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


	void PreDrawForShadow();
	
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

	void CreateRootSignatureForShadow();
	void CreateGraphicsPipelineForShadow();


	// コピーコンストラクタと演算子オーバーロードの禁止
	ModelManager(const ModelManager& obj) = delete;
	ModelManager& operator=(const ModelManager& obj) = delete;

private: // -- private メンバ変数 -- //

	//
	static std::map<std::string, std::shared_ptr<Model>> sModels_;

	// ポインタ
	DirectXCommon* mDxCommon = nullptr;
	Camera* mCamera = nullptr;

	// モデルデータ
	std::unordered_map<std::string, std::unique_ptr<Model>> models;

	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineStateForSkinning = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineStateForShadow = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignatureForSkinning = nullptr;
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignatureForForShadow = nullptr;

	// インスタンス
	static ModelManager* instance;

};

