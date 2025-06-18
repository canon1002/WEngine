#pragma once
#include "GameEngine/Scene/BaseScene.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Object/Sprite/SpriteAdministrator.h"
#include "GameEngine/UI/UIManager.h"

/// <summary>
/// メニューシーン
/// </summary>
class MenuScene :
    public BaseScene
{
public:

	// 終了処理
	void Final()override;
	// 初期化処理
	void Init() override;
	// 更新処理
	void Update() override;
	// 描画処理
	void Draw() override;
	// UI描画処理
	void DrawUI() override;

private: // -- 非公開 メンバ変数 -- //

	// メニューシーンの有効フラグ
	bool mIsActive = false;	
	
	// メニュー背景スプライト
	BaseUI* mMenuBackground;

};

