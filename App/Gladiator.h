#pragma once
#include "GameEngine/Base/Framework.h"

/// <summary>
/// 就職用個人制作作品『Gladiator』用のフレームワーククラス
/// </summary>
class Gladiator :
    public Framework
{
public: // -- 公開 メンバ関数 -- //

	// 初期化
	virtual void Init() override;
	// 更新処理
	virtual void Update() override;
	// 描画処理
	virtual void Draw() override;
	// 終了処理
	virtual void Final() override;

public: // -- 公開 メンバ変数 -- //

};

