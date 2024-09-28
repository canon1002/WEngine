#pragma once
#include "IScene.h"
#include "Input/InputManager.h"

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
	InputManager* mInput;

};

