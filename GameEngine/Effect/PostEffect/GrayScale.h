#pragma once
#include "IPostEffect.h"

/// <summary>
/// グレースケール 
/// </summary>
class GrayScale : public IPostEffect {

public:// -- 公開 メンバ関数 -- //

	GrayScale() = default;
	~GrayScale() = default;
	void Init()override;
	void Update()override;
	void PreDraw()override;
	void Draw()override;
	void PostDraw()override;
	void CreateGraphicsPipeline()override;
	void CreateRootSignature()override;

protected: // -- 限定公開 メンバ変数 -- //



};

