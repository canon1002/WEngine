#pragma once
#include "Component/Transform/WorldTransform.h"
#include "Math/Math.h"
#include "Object/Object3d.h"

class Player;

class Arrow
{
public: // -- 公開 メンバ関数 -- //

	Arrow();
	~Arrow();

	void Init(Player* player,Vector3 pos,Vector3 vel);
	void Update();
	void Draw();
	void DebugDraw() { mObject->Debug(); }

	Vector3 GetWorldPos() { return mObject->GetWorld().translation; };
	void SetCubeMap(const int32_t& textureHandle) {
		mObject->GetModel()->SetCubeTexture(textureHandle); 
		mObject->mCollider->GetModel()->mTextureHandleCubeMap = textureHandle;
	}
	void SetRotate(const Vector3& rot) { mObject->GetWorld().rotation = rot; }

	bool GetIsActive()const { return mIsActive; }
	Collider* GetCollider() { return mObject->mCollider; }

private: // -- 非公開 メンバ変数 -- //

	// プレイヤーのポインタ
	Player* mPlayer;

	// オブジェクトクラス
	std::unique_ptr<Object3d> mObject;
	// 移動量
	Vector3 mMoveVel;
	// 有効フラグ
	bool mIsActive;

};

