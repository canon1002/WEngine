#pragma once
#include "GameEngine/Scene/BaseScene.h"
#include "GameEngine/Input/InputManager.h"

class MenuScene :
    public BaseScene
{
public:
	void Final()override;

	//　継承した関数
	void Init() override;
	void Update() override;
	void Draw() override;
	void DrawUI() override;

public:
	
	// 入力マネージャー
	

	// フェード中か
	bool mIsFading;
	// 開始前のビネット
	float mViggnetTime;

};

