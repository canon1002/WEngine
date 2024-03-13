#pragma once
#include "Engine/Base/DirectXCommon.h"

class SpriteCommon
{
private: // 非公開メンバ関数

	// インスタンス
	static SpriteCommon* instance;
	// コンストラクタ
	SpriteCommon() = default;
	// デストラクタ
	~SpriteCommon() = default;


public: // メンバ関数

	// インスタンスの取得
	static SpriteCommon* GetInstance();
	// 終了処理
	void Finalize();
	void Initialize();
	void CreateRootSignature();
	void CreateGraphicsPipeline();
	void Update();
	void DrawBegin();

private: // メンバ変数

	// DirectXCommonの外部ポインタ
	DirectXCommon* dx_ = nullptr;
	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature = nullptr;

};

