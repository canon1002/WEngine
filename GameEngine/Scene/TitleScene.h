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
	
	// -- シャドウテスト -- //

	// オブジェクト
	std::unique_ptr<ShadowObject> mObject;

};

