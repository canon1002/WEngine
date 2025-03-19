#pragma once
#include "ParticleCommon.h"
#include "GameEngine/Object/Model/Model.h"
#include <unordered_map>
#include <random>
#include <list>

// パーティクルグループ
struct ParticleGroup {
	// マテリアルデータ
	MaterialParticle material;
	// UV座標
	UVTransform uvTransform;
	// マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	// パーティクルのリスト
	std::list<Particle> particleList;
	// インスタンシングデータ用SRVインデックス
	int32_t srvIndex;
	// インスタンシングリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
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
	
	// パーティクルの発生処理
	void Emit(const std::string& name, const Emitter& emitter);

	//	パーティクル 生成処理
	Particle Create(Emitter emitter);

	// 
	uint32_t SetInstancingBuffer(int32_t kNumInstance, Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource);

private: // -- 非公開 メンバ関数 -- //

	// コンストラクタ
	ParticleManager() = default;
	// デストラクタ
	~ParticleManager();
	// コピーコンストラクタと演算子オーバーロードの禁止
	ParticleManager(const ParticleManager& obj) = delete;
	ParticleManager& operator=(const ParticleManager& obj) = delete;


	// ルートシグネチャの生成
	void CreateRootSignature();
	// パイプラインステートの生成
	void CreatePipelineState();

	// 頂点リソースの生成
	void CreateVertexResource();
	// マテリアルリソースの生成
	void CreateMaterialResource(ParticleGroup& particleGroup);
	// インスタンシングリソースの生成
	void CreateInstancingResource(ParticleGroup& particleGroup);

private: // -- 非公開 メンバ変数 -- //

	// インスタンス
	static ParticleManager* instance;

	// パーティクルグループのコンテナ
	std::unordered_map<std::string,ParticleGroup> mParticleGroups;

	// インスタンスの数
	const int32_t kNumMaxInstance = 400;
	//int32_t mInstanceCount;

	// 乱数生成機
	std::random_device mSeedGenerator;
	std::mt19937 mRandomEngine;

	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> mGraphicsPipelineState = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr <ID3D12RootSignature> mRootSignature = nullptr;

	// モデルデータ
	ModelData mModelData;
	
	// 頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexResource = nullptr;
	// 頂点データ
	VertexData* mVertexData = nullptr;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};



};

