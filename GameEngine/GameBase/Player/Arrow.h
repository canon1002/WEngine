#pragma once
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Math/Math.h"
#include "GameEngine/Object/3d/Object3d.h"

class Arrow
{
public: // -- 公開 メンバ関数 -- //

	Arrow();
	~Arrow();

	void Init(Vector3 pos,Vector3 vel);
	void Update();
	void Draw();
	void DebugDraw() { mObject->DrawGUI(); }

	Vector3 GetWorldPos() { return mObject->GetWorldTransform()->translation; };
	void SetCubeMap(const int32_t& textureHandle) { mObject->GetModel()->SetCubeTexture(textureHandle); }
	void SetRotate(const Vector3& rot) { mObject->mWorldTransform->rotation = rot; }

	bool GetIsActive()const { return mIsActive; }
	Collider* GetCollider() { return mObject->mCollider; }

private: // -- 非公開 メンバ変数 -- //

	// オブジェクトクラス
	std::unique_ptr<Object3d> mObject;
	// 移動量
	Vector3 mMoveVel;
	// 有効フラグ
	bool mIsActive;

};

