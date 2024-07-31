#pragma once
#include "IScene.h"

#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"
#include "GameEngine/Object/Grid3D.h"
#include "GameEngine/Effect/Particle/DiffusionToCircleParticle.h"

// ゲーム用
#include "GameEngine/GameBase/Player/Player.h"
#include "GameEngine/GameBase/Enemy/BossEnemy.h"
// マネージャー
#include "GameEngine/Append/Collider/CollisionManager.h"
#include "GameEngine/Input/InputManager.h"

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

	Skybox* skybox_;
	std::unique_ptr<Grid3D> grid_;

	std::unique_ptr<Player> mPlayer;
	std::unique_ptr<BossEnemy> mBoss;

	// 衝突判定マネージャ
	std::unique_ptr<CollisionManager> mCollisionManager;

	// 円形拡散パーティクル
	std::unique_ptr<DiffusionToCircleParticle> mDTCParticle;

};

