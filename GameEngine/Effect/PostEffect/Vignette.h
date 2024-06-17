#pragma once
#include "IPostEffect.h"

/// 調整項目 
struct VignetteItems {
	int32_t enable;   // ビネット処理の有無(画面端を暗くする)
	float multipliier; // ビネット処理の際に使用する乗数
	float index;       // ビネット処理の際に使用する指数
};

/// <summary>
/// ビネット 
/// </summary>
class Vignette : public IPostEffect {

public:// -- 公開 メンバ関数 -- //

	Vignette() = default;
	~Vignette();
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
	VignetteItems* mItems;
};

