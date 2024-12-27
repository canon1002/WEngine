#pragma once
#include "IScene.h"
#include "GameEngine/Input/InputManager.h"

class OverScene :
    public IScene
{
public:
	void Finalize()override;

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

