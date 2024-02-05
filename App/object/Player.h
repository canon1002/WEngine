#pragma once
#include <stdint.h>
#include "../worldTransform/WorldTransform.h"
#include "../math/Math.h"
#include "../color/Color.h"
#include "../3d/Object3d.h"
#include "../collision/Collider.h"

class Player:
	public Collider
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
	
	bool GetIsActive() { return isActive_; }

public: // コリジョン

	// 純粋仮想関数
	void OnCollision()override { isActive_ = false; }
	void OnCollision(Collider* collider)override { collider; isActive_ = false; }

	// ワールド座標
	Vec3 GetWorldPos()override { return worldTransform_.translate; }

	// コライダー
	uint32_t GetCollisionAttribute()override { return collisionAttribute_; }
	// 
	void SetCollisionAttribute(uint32_t collisionAttribute)override { collisionAttribute_ = collisionAttribute; }
	// 
	uint32_t GetCollisionMask()override { return collisionMask_; };
	// 
	void SetCollisionMask(uint32_t collisionMask)override { collisionMask_ = collisionMask; }


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
	 bool isActive_ = true;

	 //
	 uint32_t collisionAttribute_ = 0xffffffff;
	 // 
	 uint32_t collisionMask_ = 0xffffffff;

};

