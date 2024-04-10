#pragma once
#include <stdint.h>
#include "Engine/Object/WorldTransform/WorldTransform.h"
#include "Engine/Math/Math.h"
#include "Engine/Object/Object3d.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Object/Sprite/Sprite.h"
#include "Engine/object/Sprite/SpriteCommon.h"
#include <memory>

class Reticle
{
public:

	Reticle() = default;
	~Reticle() = default;
	void Initialze();
	void Update();
	void Draw();
	void DrawModel();

	WorldTransform GetWorld() { return worldTransformReticle3d; }

	void SetCamera(CameraCommon* camera) {
		reticle3d->SetCamera(camera);
		camera_ = camera;
	}

private:

	// 外部ポインタ
	CameraCommon* camera_ = nullptr;

	// 2dレティクル
	std::unique_ptr<Sprite> reticle2d;
	Vec2 reticle2dPos = { 360.0f,360.0f };
	// 3dレティクル
	std::unique_ptr<Object3d> reticle3d;
	WorldTransform worldTransformReticle3d;

};

