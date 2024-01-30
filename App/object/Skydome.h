#pragma once
#include <memory>
#include "../worldTransform/WorldTransform.h"
#include "../math/Math.h"
#include "../color/Color.h"
#include "../3d/Object3d.h"

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

