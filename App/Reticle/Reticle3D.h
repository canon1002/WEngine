#pragma once
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Math/Math.h"
#include "GameEngine/Object/3d/Object3d.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Object/Sprite/Sprite.h"

class InputManager;

class Reticle3D {
public: // -- 公開 メンバ関数 -- //
	
	Reticle3D();
	~Reticle3D();

	void Init();
	void Update();
	void Draw3DReticle();
	void Draw2DReticle();

	// 3Dレティクルのワールド座標を取得する
	Vector3 GetWorld3D() { return mObject->GetWorldTransform()->translation; }
	void SetCubeMap(const int32_t& textureHandle) { mObject->GetModel()->SetCubeTexture(textureHandle); }

	void SetReticleDistance(float distance) { mReticleDistance = distance; }

	Collider* GetReticleCollider() { return mObject->mCollider; }

private: // -- 非公開 メンバ変数 -- //

	InputManager* pInput;

	// カメラのポインタ
	CameraCommon* pCamera;

	// 3Dレティクルオブジェクト
	std::unique_ptr<Object3d> mObject;

	// 2Dレティクルの座標
	Vector2 mPostionReticle2D;
	// 2Dレティクルスプライト
	std::unique_ptr<Sprite> mSprite;

	// プレイヤーから3Dレティクルへの距離
	float mReticleDistance;

	Vector3 mPosNear;
	Vector3 mPosFar;

};

