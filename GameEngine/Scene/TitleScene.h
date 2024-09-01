#pragma once
#include "IScene.h"

#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"

// テスト用
#include "GameEngine/Object/ShadowObject.h"

// ゲーム用
#include "GameEngine/GameBase/Player/Player.h"
#include "GameEngine/GameBase/Enemy/BossEnemy.h"
// エフェクト
#include "GameEngine/Effect/Particle/DiffusionToCircleParticle.h"
// マネージャー
#include "GameEngine/Append/Collider/CollisionManager.h"
#include "GameEngine/Input/InputManager.h"

// 3次元ベクトル補間
struct EasingVector3 {
	Vector3 s;	// 始点
	Vector3 e;	// 終点
	float t;	// 時間
};

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

	// 入力マネージャー
	InputManager* mInput;
	
	// カメラ
	CameraCommon* mCamera;
	// 地面
	std::unique_ptr<Object3d> mGroundObj;
	// 建物
	std::unique_ptr<Object3d> mBuildingObj;

	// ゲームシーンに移行するか
	bool mIsTransitionGameScene;
	// タイトルでの選択表示に移行するか
	bool mIsTransitionTitleSelect;

	// 何かしら遷移中であるか
	bool mIsActiveTransition;

	// スタート補間
	EasingVector3 mCameraRot;// カメラ回転
	EasingVector3 mCameraTrYZ;// カメラ移動
	EasingVector3 mCameraTrZ;// カメラ移動
	


	// -- シャドウテスト -- //

	// オブジェクト
	std::unique_ptr<ShadowObject> mObject;

};

