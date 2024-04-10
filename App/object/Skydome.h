#pragma once
#include <memory>
#include "Engine/Object/WorldTransform/WorldTransform.h"
#include "Engine/Math/Math.h"
#include "Engine/Object/Texture/Color.h"
#include "Engine/Object/Object3d.h"
#include "Engine/Collision/Collider.h"

class Skydome
{
public:

	void Initialize();
	void Update();
	void Draw();
	void SetCamera(CameraCommon* camera) { object_->SetCamera(camera); }

private:

	std::unique_ptr<Object3d> object_;
	WorldTransform worldTransform_;

};

