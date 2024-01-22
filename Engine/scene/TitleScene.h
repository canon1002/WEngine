#pragma once
#include "IScene.h"

// 前方宣言
class Input;

class TitleScene :
    public IScene
{
public:

	//　継承した関数
	void Init() override;
	void Update() override;
	void Draw() override;

private:

	// 外部ポインタ
	Input* input_ = nullptr;

};

