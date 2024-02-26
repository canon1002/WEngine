#pragma once
#include "IScene.h"
#include "../Engine/primitive/Sprite.h"
#include "../Engine/object/2d/SpriteCommon.h"
#include <memory>

class ResultScene :
    public IScene
{
public:
	void Finalize()override;

	//　継承した関数
	void Init() override;
	void Update() override;
	void Draw() override;
public:
	std::unique_ptr<Sprite> UI01 = nullptr;

	std::unique_ptr<Sprite> sprite_ = nullptr;
	float alpth_ = 0.0f;
	bool isSceneChange = false;
	int32_t fadeTimer_ = 60;
};

