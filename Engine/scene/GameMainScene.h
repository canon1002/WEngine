#pragma once
#include "IScene.h"
#include "Engine/Object/Sprite/Sprite.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Object/Camera/MainCamera.h"

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

	InputManager* input_;
	MainCamera* camera_;

	std::unique_ptr<Sprite> spriteA_ = nullptr;
	std::unique_ptr<Sprite> spriteB_ = nullptr;

};

