#pragma once
#include "GameEngine/Math/Math.h"
#include "CollisionConfig.h"
#include <list>
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"


// 衝突判定で使う数学及びアルゴリズムのインクルード
#define _USE_MATH_DEFINES
#include<cmath>
#include<algorithm>

#include "GameEngine/Append/Transform/WorldTransform.h"

#ifdef _DEBUG

#include "GameEngine/Object/Grid3D.h"
#include "GameEngine/Object/Model/Model.h"

#endif // _DEBUG


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
	virtual void Draw() = 0;
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

protected: // -- 限定公開 メンバ変数 -- //

	// 衝突属性 自分
	uint32_t mCollisionAttribute = 0xffffffff;
	// 衝突属性 相手
	uint32_t mCollisionMask = 0xffffffff;

	// -- ここから下はデバッグ用の変数 -- //

#ifdef _DEBUG

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

#endif // _DEBUG

};