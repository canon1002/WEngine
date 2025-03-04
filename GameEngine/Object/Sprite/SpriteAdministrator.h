#pragma once
#include "GameEngine/Base/DirectX/DirectXCommon.h"

class SpriteAdministrator{

private: // -- 非公開 メンバ関数 -- //

	// コンストラクタ
	SpriteAdministrator() = default;
	// デストラクタ
	~SpriteAdministrator() = default;

public: // -- 公開 メンバ関数 -- //

	// インスタンス取得
	static SpriteAdministrator* GetInstance();
	
	// 終了処理
	void Finalize();
	
	void Init();
	
	void CreateRootSignature();
	void CreateGraphicsPipeline();
	void Update();
	void PreDraw();

private: // -- 非公開 メンバ変数 -- //

	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> mGraphicsPipelineState = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr <ID3D12RootSignature> mRootSignature = nullptr;

	static SpriteAdministrator* instance;

};

