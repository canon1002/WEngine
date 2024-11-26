#pragma once
#include "ParticleCommon.h"
#include <unordered_map>

// パーティクルグループ
struct ParticleGroup {
	// マテリアルデータ
	Material* materialData;
	// パーティクルのリスト
	//std::list<ParticleCommon> particleList;
	// インスタンシングデータ用SRVインデックス
	int32_t mInstancingHandle;
	// インスタンシングリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> mInstancingResource;
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
	
	void CreateParticleGroupe(const std::string name,const std::string textureFilePath);

private: // -- 非公開 メンバ関数 -- //

	void CreateRootSignature();
	void CreatePipelineState();

private: // -- 非公開 メンバ変数 -- //

	// ポインタ
	DirectXCommon* mDxCommon;

	// インスタンス
	static ParticleManager* instance;
	// パーティクルグループのコンテナ
	std::unordered_map<std::string,ParticleGroup> mParticleGroups;

	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> mGraphicsPipelineState = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr <ID3D12RootSignature> mRootSignature = nullptr;


};

