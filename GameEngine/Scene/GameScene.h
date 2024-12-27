#pragma once
#include "IScene.h"

#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"

// ゲーム用
#include "App/Player/Player.h"
#include "App/Enemy/BossEnemy.h"

// エフェクト
#include "GameEngine/Effect/Particle/DiffusionToCircleParticle.h"
#include "GameEngine/Effect/Particle/TrailEffect.h"
#include "GameEngine/Effect/Particle/DashSmoke.h"

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

// イージング変数セット
struct EasingToVector3 {
	Vector3 start;	// 初期地点
	Vector3 end;	// 終了地点
	float t;		// 時間(0.0f ~ 1.0f)
	float k;		// 係数(指数関数用)
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

	// ヒットストップの開始
	void StartHitStop(float duration);

	// ヒットストップの更新
	void UpdateHitStop();

private:

	// 入力マネージャー
	
	// 衝突判定マネージャ
	std::unique_ptr<CollisionManager> mCollisionManager;

	// スカイボックス
	Skybox* mSkybox;
	// プレイヤーキャラ
	std::unique_ptr<Player> mPlayer;
	// 敵キャラ
	std::unique_ptr<BossEnemy> mBoss;

	// 円形拡散パーティクル
	std::unique_ptr<DiffusionToCircleParticle> mDTCParticle;

	// ゲームシーンの段階
	Phase mPhase;

	// 開始前のビネット
	float mViggnetTime;
	bool mIsGameOverSelect;
	// ゲームオーバー時の赤色スプライト
	std::unique_ptr<Sprite> mGameOverFadeSprite;
	std::unique_ptr<Sprite> mGameOverMessageSprite;
	float mMessageFadeTime;
	std::array<std::unique_ptr<Sprite>, 2> mGameOverSelectUI;
	int32_t mGameOverSelectUICount;

	// ゲーム開始演出関連
	EasingToVector3 mPlayerStartAndEnd;
	EasingToVector3 mBossStartAndEnd;
	EasingToVector3 mCameraRot;
	EasingToVector3 mCameraTr;

	// UI
	UISet mMoveUI;
	UISet mActionUI;

	// 動くオブジェクトの地面影
	std::array<std::unique_ptr<Object3d>, 2> mGroundShadow;


	// ゲームクリア演出
	UISet mFinishUI;
	float mFinithUIDisplsyTime;
	const float kFinithUIDisplsyTimeMax = 4.0f;
	bool mIsFinishUIDisplayEnd;

	// ヒットストップ・スロー関連のメンバ変数
	bool mIsHitStopActive = false;
	float mHitStopDuration = 0.0f;
	float mHitStopTimer = 0.0f;


	// 軌道パーティクル
	std::unique_ptr<TrailEffect> mPlayerTrailEffect;// プレイヤー用
	std::unique_ptr<TrailEffect> mBossTrailEffect;
	// ダッシュ煙
	std::unique_ptr<DashSmoke> mPlayerDashSmoke; // プレイヤー用
	std::unique_ptr<DashSmoke> mBossDashSmoke; // ボス用

};

