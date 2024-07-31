#pragma once
#include "IPostEffect.h"

class RenderImage : public IPostEffect {

public:// -- 公開 メンバ関数 -- //

	RenderImage() = default;
	~RenderImage();
	void Init()override;
	void Update()override;
	void DrawGUI()override;
	void PreDraw()override;
	void Draw()override;
	void PostDraw()override;

private: // -- 非公開 メンバ関数 -- //

	// グラフィックスパイプラインを生成
	void CreateGraphicsPipeline()override;
	// ルートシグネチャを生成
	void CreateRootSignature()override;

protected: // -- 限定公開 メンバ変数 -- //

};

