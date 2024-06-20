#pragma once
#include "IPostEffect.h"
#include "RenderImage.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"

/// <summary>
/// ポストエフェクト 管理者クラス
/// </summary>
class PostEffectManager{
public:// -- 公開 メンバ関数 -- //

	PostEffectManager() = default;
	~PostEffectManager();
	void Init();
	void Update();
	void Draw();
	void RenderFinalOutput();

private: // -- 非公開 メンバ関数 -- //

	// 中間テクスチャの生成を行う
	void CreateIntermediateTextures();

	

private: // -- 非公開 メンバ変数 -- //

	// DirectXCommonのポインタ
	DirectXCommon* mDxCommon;

	// ポストエフェクトの配列
	std::vector<IPostEffect*> mPostEffects;

	// ポストエフェクト最終結果
	RenderImage* mFinalRender;

	// 中間テクスチャ(2個作成する)
	int32_t mIntermediateTextures[2];



};

