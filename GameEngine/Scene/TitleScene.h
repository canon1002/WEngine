#pragma once
#include "IScene.h"
#include "GameEngine/Object/3d/Object3d.h"
#include "GameEngine/Object/Particle/VoxelParticle.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Sprite/Sprite.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"
#include "GameEngine/Object/Model/Line.h"
#include "GameEngine/Object/Grid3D.h"

#include "GameEngine/GameBase/Player/Player.h"
#include "GameEngine/GameBase/Enemy/BossEnemy.h"

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

	// 入力を取得
	InputManager* mInput;

	std::unique_ptr<Object3d> testObject02_;
	std::unique_ptr<Skybox> skybox_;
	std::unique_ptr<Grid3D> grid_;

	std::unique_ptr<Player> mPlayer;
	std::unique_ptr<BossEnemy> mBoss;
	
};

