#pragma once
#include <stdint.h>
#include "Engine/Object/WorldTransform/WorldTransform.h"
#include "Engine/Math/Math.h"
#include "Engine/Object/Texture/Color.h"
#include "Engine/Object/Object3d.h"
#include "Engine/Collision/Collider.h"
#include "App/object/Reticle.h"

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
		//reticle->SetCamera(camera);
		camera_= camera;
	}
	
	bool GetIsActive() { return isActive_; }

	Reticle* GetReticle() { return reticle.get(); }
	Vec3 GetReticleAxis();

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
	void AddRotate(Vec3 rotation) { worldTransform_.rotate += rotation; }

private:

	// 参照用
	CameraCommon* camera_ = nullptr;
	
	
	std::unique_ptr<Object3d> object_;
	WorldTransform worldTransform_;
	WorldTransform worldTransformRail_;
	 Vec3 rad_ = { 1,1,1 };
	 Vec3 vel_ = { 0.1f,0.1f,0.1f };
	 Vec3 speed_ = { 4,4,4 };
	 Color color_ = { 1.0f,1.0f,1.0f,1.0f };
	 bool isActive_ = true;

	 //
	 uint32_t collisionAttribute_ = kCollisionAttributePlayer;
	 // 
	 uint32_t collisionMask_ = kCollisionAttributeEnemy + kCollisionAttributeEnemyBullet;

	 // レティクル
	 std::unique_ptr<Reticle> reticle;

};

