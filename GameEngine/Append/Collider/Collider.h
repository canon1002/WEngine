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

#include "GameEngine/Object/Grid3D.h"
#include "GameEngine/Object/Model/Model.h"

struct AABB {
	Vector3 min;// 最小値
	Vector3 max;// 最大値
};

struct Sphere {
	Vector3 center;// 中心
	float radius;// 半径
};

struct Plane {
	Vector3 normal;// 法線(単位ベクトル)
	float distance;// 距離
};

class AABBCollider;
class SphereCollider;
class OBBCollider;

/// <summary>
///	コライダー
/// <summary>
class Collider
{
public: // -- 公開 メンバ関数 -- //

	// コンストラクタ
	Collider() = default;

	// 仮想デストラクタ
	virtual ~Collider() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() = 0;
	
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;
	
	/// <summary>
	/// 描画処理(デバッグ用)
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 衝突時の処理
	/// </summary>
	virtual void OnCollision() = 0;
	
	/// <summary>
	/// 衝突時の処理 
	/// </summary>
	/// <param name="collider">衝突対象</param>
	virtual void OnCollision(Collider* collider) = 0;

	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <returns>ワールド座標</returns>
	virtual Vector3 GetWorldPos() const  = 0;
	
	/// <summary>
	/// ワールドトランスフォームの取得
	/// </summary>
	/// <returns>ワールドトランスフォームのポインタ</returns>
	WorldTransform* GetWorld() { return mWorldTransform; }

	// 種別IDを取得
	uint32_t GetTypeID()const { return mTypeID; }
	// 種別IDを設定
	void SetTypeID(uint32_t typeID) { mTypeID = typeID; }

	// コライダー同士の衝突判定(マネージャで呼び出す用)
	virtual bool IsCollision(Collider* c) = 0;
	virtual bool IsCollision(AABBCollider* c) = 0;
	virtual bool IsCollision(SphereCollider* c) = 0;
	virtual bool IsCollision(OBBCollider* c) = 0;

	
	Model* GetModel() const{ return mModel.get(); }
	void DebugDraw(std::string label) { mModel->DrawGUI(label); }

	// 衝突フラグ取得
	virtual bool GetOnCollisionFlag()const { return mIsOnCollision; }

	// ワールド座標の登録
	void SetWorld(WorldTransform* world) {
		mWorldTransform = world;
	}
	// 追加の平行移動量を設定
	void SetAddTranslation(Vector3 translation) { mAddtranslation = translation; }
	
protected: // -- 限定公開 メンバ関数 -- //

	/// <summary>
	/// デバッグ用の座標リソースの生成
	/// </summary>
	virtual void CreateTransformation() = 0;


protected: // -- 限定公開 メンバ変数 -- //

	// 種別ID
	uint32_t mTypeID = 0u;
	// 追加平行移動量
	Vector3 mAddtranslation;

	// 衝突フラグ
	bool mIsOnCollision;
	// 衝突フラグ解消までの時間
	int32_t mOnCollisionCount;

	// -- ここから下はデバッグ用の変数 -- //

	Matrix4x4 viewM, wvpM;
	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mWvpResource = nullptr;
	// データを書き込む
	std::shared_ptr<TransformationMatrixForGrid3D> mWvpData = nullptr;
	WorldTransform* mWorldTransform;
	// モデル
	std::shared_ptr<Model> mModel;

};