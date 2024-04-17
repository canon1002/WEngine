#pragma once
#include "IScene.h"
#include "GameEngine/Object/Sprite/Sprite.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Object/Camera/MainCamera.h"

class GameScene :
	public IScene
{
public:

	~GameScene() {}
	//　継承した関数
	void Finalize()override;
	void Init() override;
	void Update() override;
	void Draw() override;

private:

};

