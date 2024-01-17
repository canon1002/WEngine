#pragma once
#include "IScene.h"
class ResultScene :
    public IScene
{
public:

	//　継承した関数
	void Init() override;
	void Update() override;
	void Draw() override;

};

