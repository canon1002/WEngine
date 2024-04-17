#pragma once
#include "GameEngine/Base/DirectX/DirectXCommon.h"

class SpriteAdministrator{

public: // メンバ関数


	// コンストラクタ
	SpriteAdministrator() = default;
	// デストラクタ
	~SpriteAdministrator() = default;
	
	// 終了処理
	void Finalize();
	
	void Initialize(DirectXCommon* dxCommon);
	
	void CreateRootSignature();
	void CreateGraphicsPipeline();
	void Update();
	void PostDraw();

private: // メンバ変数

	// DirectXCommonの外部ポインタ
	DirectXCommon* dxCommon_ = nullptr;
	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature = nullptr;

};

