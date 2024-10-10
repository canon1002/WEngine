#pragma once
#include "Math/Math.h"
#include "CollisionConfig.h"
#include <list>
#include "Base/DirectX/DirectXCommon.h"
#include "Resource/Base/Resource.h"


// 衝突判定で使う数学及びアルゴリズムのインクルード
#define _USE_MATH_DEFINES
#include<cmath>
#include<algorithm>

#include "Engine/Object/Camera/Camera.h"
#include "Component/Transform/WorldTransform.h"
#include "Resource/Model/Model.h"



struct Plane {
	Vector3 normal;// 法線(単位ベクトル)
	float distance;// 距離
};

class AABBCollider;
class SphereCollider;

/// <summary>
///	コライダー
/// <summary>
class Collider
{
public: // -- 公開 メンバ関数 -- //


	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw(Camera camera) = 0;
	virtual void CreateTransformation() = 0;

	// 衝突時の処理
	virtual void OnCollision() = 0;
	// 衝突時の処理 
	virtual void OnCollision(Collider* collider) = 0;
	


	// ワールド座標の取得
	virtual Vector3 GetWorldPos() const  = 0;
	
	// 衝突属性の変更・取得
	inline uint32_t GetCollisionAttribute() { return mCollisionAttribute; }
	inline void SetCollisionAttribute(uint32_t collisionAttribute) { mCollisionAttribute = collisionAttribute; }
	inline uint32_t GetCollisionMask() { return mCollisionMask; }
	inline void SetCollisionMask(uint32_t collisionMask) { mCollisionMask = collisionMask; }

	// コライダー同士の衝突判定(マネージャで呼び出す用)
	virtual bool IsCollision(Collider* c) = 0;
	virtual bool IsCollision(AABBCollider* c) = 0;
	virtual bool IsCollision(SphereCollider* c) = 0;

	WorldTransform* GetWorld() { return pWorldTransform; }
	Model* GetModel() const{ return mModel; }
	void DebugDraw(std::string label) { mModel->DrawGUI(label); }

	// 衝突フラグ取得
	virtual bool GetOnCollisionFlag()const { return mIsOnCollision; }

	void SetWorld(WorldTransform* world) { pWorldTransform = world; }
	void SetAddtranslation(Vector3 translation) { mAddtranslation = translation; }
	

protected: // -- 限定公開 メンバ変数 -- //

	// 衝突属性 自分
	uint32_t mCollisionAttribute = 0xffffffff;
	// 衝突属性 相手
	uint32_t mCollisionMask = 0xffffffff;

	// 追加平行移動量
	Vector3 mAddtranslation;

	// -- ここから下はデバッグ用の変数 -- //


	// 外部ポインタ
	DirectXCommon* mDxCommon = nullptr;
	Matrix4x4 viewM, wvpM;

	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mWvpResource = nullptr;
	// データを書き込む
	TransformationMatrixForGrid3D* mWvpData = nullptr;
	WorldTransform* pWorldTransform;

	// モデル
	Model* mModel;

	// 衝突フラグ
	bool mIsOnCollision;
	// 衝突フラグ解消までの時間
	int32_t mOnCollisionCount;

};