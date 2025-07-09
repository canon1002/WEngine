#pragma once
#include <unordered_map>
#include <map>
#include <sstream>
#include "Model.h"
#include "GameEngine/Resource/Material/Material.h"

/// <summary>
/// 3Dモデル管理クラス
/// </summary>
class ModelManager{

public: // -- 公開 メンバ関数 -- //

	// コンストラクタ
	ModelManager() = default;
	// デストラクタ
	~ModelManager() = default;

	// インスタンス取得関数
	static ModelManager* GetInstance();
	// 終了処理
	void Finalize();
	// 初期化
	void Init();
	//描画前処理
	void PreDraw(); // 通常3Dモデル用
	void PreDrawForSkinning(); // スキニング3Dモデル用
	void PreDrawForShadow(); // シャドウマップ用の描画前処理
	// 描画
	void Draw();

	// -- 生成処理 -- //

	// モデルの生成 & ポインタ取得
	Model* CreateModel(const std::string& directoryPath, const std::string& filepath);

	// -- 読み込み処理 -- //

	// モデルの読み込み
	void LoadModel(const std::string& directoryPath, const std::string& filepath);
	
	// -- 検索・取得処理 -- //

	// モデルの取得
	Model* FindModel(const std::string filepath);


private: // -- 非公開 メンバ関数 -- //

	// コピーコンストラクタと演算子オーバーロードの禁止
	ModelManager(const ModelManager& obj) = delete;
	ModelManager& operator=(const ModelManager& obj) = delete;


	// 3Dモデル用のルートシグネチャを生成
	void CreateRootSignature();
	// 3Dモデル用のグラフィックスパイプラインを生成
	void CreateGraphicsPipeline();

	// Skinning用のルートシグネチャを生成
	void CreateRootSignatureForSkinning();
	// Skinning用のグラフィックスパイプラインを生成
	void CreateGraphicsPipelineForSkinning();

	// シャドウマップ用のルートシグネチャを生成
	void CreateRootSignatureForShadow();
	// シャドウマップ用のグラフィックスパイプラインを生成
	void CreateGraphicsPipelineForShadow();


private: // -- private メンバ変数 -- //

	// モデルデータマップ
	std::unordered_map<std::string, std::shared_ptr<Model>> mModels;

	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineStateForSkinning = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineStateForShadow = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignatureForSkinning = nullptr;
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignatureForForShadow = nullptr;

	// インスタンス
	static std::unique_ptr<ModelManager> instance;

};

