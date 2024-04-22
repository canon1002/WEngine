#pragma once
#include "IScene.h"
#include "GameEngine/Object/3d/Object3d.h"
#include "GameEngine/Object/Particle/VoxelParticle.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Sprite/Sprite.h"

class TitleScene :
    public IScene
{
public:
	~TitleScene() {}

	void Finalize()override;

	//　継承した関数
	void Init() override;
	void Update() override;
	void Draw() override;

private:

	std::shared_ptr<Object3d> testObject_;

};

