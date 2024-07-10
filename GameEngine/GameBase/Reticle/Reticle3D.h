#pragma once
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Math/Math.h"
#include "GameEngine/Object/3d/Object3d.h"
#include "GameEngine/Input/InputManager.h"

class InputManager;

class Reticle3D {
public: // -- 公開 メンバ関数 -- //
	
	Reticle3D();
	~Reticle3D();

	void Init();
	void Update();
	void Draw3DReticle();
	void Draw2DReticle();

private: // -- 非公開 メンバ変数 -- //

	InputManager* pInput;

	// カメラのポインタ
	CameraCommon* pCamera;

	// 3Dレティクルのワールド座標
	WorldTransform* mWorldReticle3D;
	// 2Dレティクルの座標
	Vec2 mPostionReticle2D;



};

