#pragma once
#include "GameEngine/Scene/BaseScene.h"

#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Skybox/Skybox.h"

// ゲーム用
#include "App/Player/Player.h"
#include "App/Enemy/BossEnemy.h"

// エフェクト
#include "GameEngine/Effect/Particle/ParticleManager.h"
#include "GameEngine/Effect/Particle/Emitter/ParticleEmitter.h"
#include "GameEngine/Effect/Particle/TrailEffect.h"

// マネージャー
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/UI/UIManager.h"

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
	public BaseScene
{
public: // -- 公開 メンバ関数 -- //

	// デストラクタ
	~GameScene() {};
	// 終了処理
	void Final()override;
	// 初期化
	void Init() override;
	// 更新処理
	void Update() override;
	// 描画処理
	void Draw() override;
	// UIの描画処理
	void DrawUI() override;

	// ヒットストップの開始
	void StartHitStop(float duration);

private: // -- 非公開 メンバ関数 -- //

	// 初期化処理の分割
	void InitActor(); // アクターの初期化
	void InitUI(); // UIの初期化
	void InitCamera(); // カメラの初期化
	void InitEffects(); // エフェクトの初期化
	void InitObject(); // その他オブジェクトの初期化

	// 各フェーズをメンバ関数ポインタで管理する
	void (GameScene::* mPhaseFunc)();
	void BeginPhase(); // 開始前フェーズ
	void BattlePhase(); // 戦闘中フェーズ
	void LosePhase(); // プレイヤー敗北フェーズ
	void WinPhase(); // プレイヤー勝利フェーズ
	void TutorialPhase(); // チュートリアルフェーズ

	// ヒットストップの更新
	void UpdateHitStop();

private: // -- 非公開 メンバ変数 -- //

	// ゲームシーンの段階
	Phase mPhase;


	// プレイヤーキャラ
	std::unique_ptr<Player> mPlayer;
	// 敵キャラ
	std::unique_ptr<BossEnemy> mBoss;



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

	// -- UI -- //

	

	// Pauseボタン
	BaseUI* mPauseButtonUI;

	// 演出中、スクリーンの上下に表示する黒画像
	std::array<BaseUI*, 2> mMovieScreen;


	// ボタンの間隔
	const Vector2 kButtonSpacing = { 20.0f,30.0f };
	// 文字の間隔
	const float kActionSpacing = 15.0f;

	// ボタン入力の表示UI
	std::array<std::shared_ptr<Sprite>, 5> mButtonUI;
	// 行動内容表示UI
	std::array<std::shared_ptr<Sprite>, 5> mActionsUI;


	// 動くオブジェクトの地面影
	std::array<std::unique_ptr<Object3d>, 2> mGroundShadow;


	// ゲームクリア演出
	UISet mFinishUI;
	float mFinithUIDisplsyTime;
	const float kFinithUIDisplsyTimeMax = 4.0f;
	bool mIsFinishUIDisplayEnd;

	// ヒットストップ・スロー関連のメンバ変数

	// ヒットストップ中か
	bool mIsHitStopActive = false;
	// ヒットストップの発生期間
	float mHitStopDuration = 0.0f;
	// ヒットストップが開始されてからどのくらい経ったか
	float mHitStopTimer = 0.0f;


	// 軌道パーティクル
	std::unique_ptr<TrailEffect> mPlayerTrailEffect;// プレイヤー用
	std::unique_ptr<TrailEffect> mBossTrailEffect;

	// ダッシュ煙
	std::unique_ptr<ParticleEmitter> mDashSmoke;
	// 炎パーティクル
	std::unique_ptr<ParticleEmitter> mFlameParticle;
	// ヒット時のパーティクル
	std::unique_ptr<ParticleEmitter> mHitParticle;
	// ヒット時のパーティクル(敵からの攻撃用)
	std::unique_ptr<ParticleEmitter> mEnemyHitParticle;

};
