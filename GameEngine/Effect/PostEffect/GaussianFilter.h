#pragma once
#include "IPostEffect.h"

/// 調整項目 
struct GaussianFilterItems {
	int32_t enable; // Fillterの有無
	int32_t kernelSize; // カーネルの大きさ
	float sigma;    // 標準偏差
};


/// <summary>
/// ガウスぼかし 
/// </summary>
class GaussianFilter : public IPostEffect {

public:// -- 公開 メンバ関数 -- //

	GaussianFilter() = default;
	~GaussianFilter();
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
	GaussianFilterItems* mItems;
};

