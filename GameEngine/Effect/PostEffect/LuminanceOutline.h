#pragma once
#include "IPostEffect.h"

// 調整項目
struct LuminanceOutlineItems {
	int32_t enable; // アウトラインの有無
	float multipliier;   // アウトライン生成時の差を大きくするための数値  
};

/// <summary>
/// 輝度検出アウトライン 
/// </summary>
class LuminanceOutline : public IPostEffect {

public:// -- 公開 メンバ関数 -- //

	LuminanceOutline() = default;
	~LuminanceOutline();
	void Init()override;
	void Update()override;
	void DrawGUI()override;
	void PreDraw()override;
	void Draw()override;
	void PostDraw()override;

private: // -- 非公開 メンバ関数 -- //

	/// ポストエフェクトで使うリソースを生成
	void CreateEffectResource();
	// グラフィックスパイプラインを生成
	void CreateGraphicsPipeline()override;
	// ルートシグネチャを生成
	void CreateRootSignature()override;
	
protected: // -- 限定公開 メンバ変数 -- //

	// ポストエフェクト用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> mEffectResource;
	// 調整項目
	LuminanceOutlineItems* mItems;
};

