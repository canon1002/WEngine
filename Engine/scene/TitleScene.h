#pragma once
#include "IScene.h"
#include "Engine/Object/Object3d.h"
#include "Engine/Object/Particle/VoxelParticle.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Object/Camera/MainCamera.h"

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

	InputManager* input_;
	MainCamera* camera_;

	// モデル
	std::unique_ptr<Object3d> eAxis_;
	std::unique_ptr<Object3d> ball_;

	// パーティクル
	std::unique_ptr<VoxelParticle> particleVox = { nullptr };
	std::unique_ptr<VoxelParticle> particleCiacle = { nullptr };
	
};

