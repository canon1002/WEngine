#pragma once
#include "IScene.h"

#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"

// ゲーム用
#include "App/Player/Player.h"
#include "App/Enemy/BossEnemy.h"
// エフェクト
#include "GameEngine/Effect/Particle/DiffusionToCircleParticle.h"
// マネージャー
#include "GameEngine/Append/Collider/CollisionManager.h"
#include "GameEngine/Input/InputManager.h"

// 段階
enum class Phase {
	BEGIN,	// 開始前
	BATTLE,	// 戦闘中
	LOSE,	// プレイヤー敗北
	WIN,	// プレイヤー勝利
};



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
	void DrawUI() override;

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

	// ゲームシーンの段階
	Phase mPhase;

	// 開始前のビネット
	float viggnetTime;

	// UI
	UISet mMoveUI;
	UISet mActionUI;

};

