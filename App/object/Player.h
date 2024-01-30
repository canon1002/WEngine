#pragma once
#include <stdint.h>
#include "../worldTransform/WorldTransform.h"
#include "../math/Math.h"
#include "../color/Color.h"
#include "../3d/Object3d.h"

class Player
{
public:

	Player();
	~Player();
	void Init();
	void Update();
	void Draw();

	void MoveUp();
	void MoveUpLeft();
	void MoveUpRight();
	void MoveDown();
	void MoveDownLeft();
	void MoveDownRight();
	void MoveLeft();
	void MoveRight();

	WorldTransform GetWorld() { return worldTransform_; }
	Vec3 GetWorldRailT() { return { worldTransformRail_.worldM.m[3][0],
		worldTransformRail_.worldM.m[3][1], worldTransformRail_.worldM.m[3][2], }; }

	void SetCamera(CameraCommon* camera) {
		object_->SetCamera(camera); 
		camera_= camera;
	}

private:

	// 参照用
	CameraCommon* camera_ = nullptr;
	
	
	std::unique_ptr<Object3d> object_;
	WorldTransform worldTransform_;
	WorldTransform worldTransformRail_;
	 Vec3 rad_;
	 Vec3 vel_;
	 Vec3 speed_;
	 Color color_;

};

