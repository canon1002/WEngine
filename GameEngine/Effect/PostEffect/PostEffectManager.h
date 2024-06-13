#pragma once
#include "IPostEffect.h"

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

private: // -- 非公開 メンバ変数 -- //

	// ポストエフェクトの配列
	std::vector<IPostEffect*> mPostEffects;

};

