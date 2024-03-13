#pragma once
#include "IScene.h"

class GameMainScene :
	public IScene
{
public:

	~GameMainScene() {}
	//　継承した関数
	void Finalize()override;
	void Init() override;
	void Update() override;
	void Draw() override;

private:


};

