#pragma once
#include "IPostEffect.h"



/// <summary>
/// グレースケール 
/// </summary>
class Grayscale : public IPostEffect {

public:// -- 公開 メンバ関数 -- //

	Grayscale() = default;
	~Grayscale();
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
	// ポストエフェクトの有無を設定(シェーダーに渡すのでint型)
	int32_t* mEnableEffect;
};

