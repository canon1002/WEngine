#pragma once
#include "IPostEffect.h"



/// <summary>
/// グレースケール 
/// </summary>
class DepthOutline : public IPostEffect {

public:// -- 公開 メンバ関数 -- //

	DepthOutline() = default;
	~DepthOutline();
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
	int32_t* mEnableEffect;
};

