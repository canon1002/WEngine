#pragma once
#include "IScene.h"
#include "GameEngine/Input/InputManager.h"

class StartScene :
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
	

	// 開始時の間
	const float kStartWaitTimeMax = 2.0f;
	float mStartWaitTime;

	UISet mBackGround;
	UISet mLogoImage;

	// フェード中か
	bool mIsFading;
	bool mIsFadeOuting;

	// 開始前のビネット
	float mViggnetTime;

};

