#pragma once
#include "GameEngine/Scene/BaseScene.h"

#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Skybox/Skybox.h"

// テスト用
#include "GameEngine/Object/ShadowObject.h"
// ゲーム用
#include"App/Actor/Actor.h"
#include "App/Player/Player.h"
#include "App/Enemy/BossEnemy.h"
#include "App/UI/FadeUI.h"
// エフェクト
#include "GameEngine/Effect/Particle/Emitter/ParticleEmitter.h"
// マネージャー
#include "GameEngine/Component/Collider/CollisionManager.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/UI/UIManager.h"

// 選択段階の判別用
enum class SelectStep {
	SCENESTART,	// シーン開始時
	START,		// 開始直後のボタンを押して貰う場面
	GAMESELECT,	// ゲームセレクト(開始/システム/終了 の選択場面)
	GAMESTART,	// ゲーム開始までの移行中
	UIFADE,	// UIフェード処理中
	END,// シーン終了時
};

// タイトルシーン
class TitleScene :
    public BaseScene
{
public: // -- 公開 メンバ関数 -- //

	// デストラクタ
	~TitleScene() {}
	// 終了処理
	void Final()override;
	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	// UI描画
	void DrawUI() override;

	// フェーズごとに処理を分離
	void StartPhase();	// タイトルシーン開始時の処理
	void GameSelectPhase();	// ゲームセレクト(開始/システム/終了)の選択処理
	void GameStartPhase();	// ゲーム開始までの処理
	void UIFadePhase();	// UIフェード処理

	// セレクト時の上下操作
	void SelectInputProcess();
	

private: // -- 非公開 メンバ変数 -- //
	
	// フェーズごとに処理を分離するための関数ポインタ
	void (TitleScene::* mPhaseFunc)();

	// カメラ
	CameraCommon* mCamera;
	// 地面
	std::unique_ptr<Object3d> mGroundObj;
	// プレイヤーオブジェクト
	std::unique_ptr<Object3d> mPlayerObj;
	// 剣
	std::unique_ptr<Object3d> mSwordObj;
	Matrix4x4 mWeaponParentMat;

	// 焚き火
	std::unique_ptr<Object3d> mWoodObj;
	// 炎パーティクル
	std::unique_ptr<ParticleEmitter> mFlameParticle;

	// シーン内選択段階
	SelectStep mSelectStep;
	// セレクト結果
	int32_t mSelectResult;	// 0:デフォルト(未選択) 1:ゲーム開始, 2:システム, 3:終了


	float mGameStartVignnetingTime;

	// シーン開幕のビネット処理
	bool mIsTransitionForPreScene;
	float viggnetOnlyTime;
	// 何かしら遷移中であるか
	bool mIsActiveTransition;

	// タイトルロゴ (タイトル名を記載する)
	std::weak_ptr<FadeUI> mTitleLogo;
	// スタート誘導UI (例:ボタンを押してください)
	std::weak_ptr<FadeUI> mPushStartUI;
	// UI - スタート誘導UI背景
	std::weak_ptr<FadeUI> mPushStartBackUI;
	// UI - ゲーム開始
	std::weak_ptr<FadeUI> mGameStartUI;
	// UI - システム(オプション)
	std::weak_ptr<FadeUI> mSystemUI;
	// UI - ゲーム終了
	std::weak_ptr<FadeUI> mQuitUI;
	// UI - 選択中のUI
	std::weak_ptr<FadeUI> mSelectingBackUI;
	

	// UI関連の変数
	int32_t mUISelectingNum;	// 選択中のUI番号
	
};

