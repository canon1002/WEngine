#pragma once
#include "IScene.h"

#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"

// ゲーム用
#include "GameEngine/GameBase/Player/Player.h"
#include "GameEngine/GameBase/Enemy/BossEnemy.h"
// エフェクト
#include "GameEngine/Effect/Particle/DiffusionToCircleParticle.h"
// マネージャー
#include "GameEngine/Append/Collider/CollisionManager.h"
#include "GameEngine/Input/InputManager.h"

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

	// 入力マネージャー
	InputManager* mInput;
	// 衝突判定マネージャ
	std::unique_ptr<CollisionManager> mCollisionManager;

	// スカイボックス
	Skybox* skybox_;
	// プレイヤーキャラ
	std::unique_ptr<Player> mPlayer;
	// 敵キャラ
	std::unique_ptr<BossEnemy> mBoss;

	// 円形拡散パーティクル
	std::unique_ptr<DiffusionToCircleParticle> mDTCParticle;

};

