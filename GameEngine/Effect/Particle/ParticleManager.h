#pragma once
#include "ParticleCommon.h"

class ParticleManager{

public: // -- 公開 メンバ変数 -- //

	// インスタンス取得
	static ParticleManager* GetInstance();

	void Init();
	void Update();
	void PreDraw();
	void Draw();
	
	void CreateParticle();

private: // -- 非公開 メンバ関数 -- //

	void CreateRootSignature();
	void CreatePipelineState();

private: // -- 非公開 メンバ変数 -- //

	// ポインタ
	DirectXCommon* mDxCommon;

	// インスタンス
	static ParticleManager* instance;
	// パーティクルのリスト
	std::list<ParticleCommon*> mParticles;

	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> mGraphicsPipelineState = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr <ID3D12RootSignature> mRootSignature = nullptr;


};

