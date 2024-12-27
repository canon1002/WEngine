#pragma once
#include "IScene.h"
#include "GameEngine/Input/InputManager.h"

class ResultScene :
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
	

	UISet mTitleOne;
	UISet mTitleLogo;

	// フェード中か
	bool mIsFading;
	bool mIsFadingSceneEnd;
	// 開始前のビネット
	float mViggnetTime;

};

