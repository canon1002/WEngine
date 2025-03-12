#pragma once
#include "GameEngine/Scene/BaseScene.h"

#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"

// テスト用
#include "GameEngine/Object/ShadowObject.h"
#include "GameEngine/Editor/NodeEditor/BTNodeEditor.h"

// ゲーム用
#include"App/Actor/Actor.h"
#include "App/Player/Player.h"
#include "App/Enemy/BossEnemy.h"
// エフェクト
#include "GameEngine/Effect/Particle/Emitter/ParticleEmitter.h"
// マネージャー
#include "GameEngine/Append/Collider/CollisionManager.h"
#include "GameEngine/Input/InputManager.h"

// 選択段階の判別用
enum class SelectStep {
	SCENESTART,	// シーン開始時
	START,		// 開始直後のボタンを押して貰う場面
	GAMESELECT,	// ゲームセレクト(開始/システム/終了 の選択場面)
	GAMESTART,	// ゲーム開始までの移行中
};

class TitleScene :
    public BaseScene
{
public:
	~TitleScene() {}

	void Final()override;

	//　継承した関数
	void Init() override;
	void Update() override;
	void Draw() override;
	void DrawUI() override;

private:
	
	// カメラ
	CameraCommon* mCamera;
	// スカイボックス
	Skybox* mSkybox;
	// 地面
	std::unique_ptr<Object3d> mGroundObj;
	// プレイヤーオブジェクト
	std::unique_ptr<Object3d> mPlayerObj;
	// 剣
	std::unique_ptr<Object3d> mSwordObj;
	Matrix4x4 mWeaponParentMat;

	// シーン内選択段階
	SelectStep mSelectStep;

	float mGameStartVignnetingTime;

	// シーン開幕のビネット処理
	bool mIsTransitionForPreScene;
	float viggnetOnlyTime;
	// 何かしら遷移中であるか
	bool mIsActiveTransition;

	// タイトルロゴ (タイトル名を記載する)
	UISet mTitleLogo;
	// スタート誘導UI (例:ボタンを押してください)
	UISet mPushStartUI;
	// UI - スタート誘導UI背景
	UISet mPushStartBackUI;
	// UI - ゲーム開始
	UISet mGameStartUI;
	// UI - システム(オプション)
	UISet mSystemUI;
	// UI - ゲーム終了
	UISet mQuitUI;
	// UI - 選択中のUI
	UISet mSelectingBackUI;
	

	// UI関連の変数
	const float kUIPositionSpace = 40.0f;	// 各UI(選択肢)の間隔
	const float kUIBasePosition = 480.0f;	// UI(選択肢)の基本となる座標
	
	int32_t mUISelectingNum;	// 選択中のUI番号
	bool mIsUpperBackUICount;	// 背景UIのカウントが上昇中であるか

	bool mIsTransUI;	// UI表示の遷移中であるか
	float mUITransCount;	// UI表示切替の進行度

	// ダッシュ煙 テスト
	std::unique_ptr<ParticleEmitter> mDashSomke;
	std::unique_ptr<Object3d> mUVChecker;

	// -- エディタテスト -- //
	std::unique_ptr<Actor> mBTNodeTestActor;
	std::unique_ptr<BTNodeEditor> mBTNodeEditor;

	
};

