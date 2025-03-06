#pragma once
#include "ParticleCommon.h"
#include <unordered_map>

// パーティクルグループ
struct ParticleGroup {
	// マテリアルデータ
	Material* materialData;
	// パーティクルのリスト
	std::list<ParticleCommon> particleList;
	// インスタンシングデータ用SRVインデックス
	int32_t srvIndex;
	// インスタンシングリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
	// データを書き込む
	// インスタンス数
	int32_t instanceCount;
	// インスタンシングデータを書き込むためのポインタ
	ParticleForGPU* instancingData;
};

class ParticleManager{

public: // -- 公開 メンバ変数 -- //

	// インスタンス取得
	static ParticleManager* GetInstance();

	void Init();
	void Update();
	void PreDraw();
	void Draw();
	
	// パーティクルグループの生成
	void CreateParticleGroupe(const std::string name,const std::string textureFilePath);

private: // -- 非公開 メンバ関数 -- //

	// コンストラクタ
	ParticleManager() = default;
	// デストラクタ
	~ParticleManager();
	// コピーコンストラクタと演算子オーバーロードの禁止
	ParticleManager(const ParticleManager& obj) = delete;
	ParticleManager& operator=(const ParticleManager& obj) = delete;

	void CreateRootSignature();
	void CreatePipelineState();

private: // -- 非公開 メンバ変数 -- //

	// ポインタ
	

	// インスタンス
	static ParticleManager* instance;

	// パーティクルグループのコンテナ
	std::unordered_map<std::string,ParticleGroup> mParticleGroups;

	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> mGraphicsPipelineState = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr <ID3D12RootSignature> mRootSignature = nullptr;


};

