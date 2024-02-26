#pragma once
#include <stdint.h>
#include "../worldTransform/WorldTransform.h"
#include "../base/DirectXCommon.h"
#include "../math/Math.h"
#include "../object/3d/Object3d.h"
#include "../primitive/Sprite.h"
#include "../Engine/object/2d/SpriteCommon.h"
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

