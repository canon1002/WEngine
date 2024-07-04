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

/// <summary>
///	コライダー
/// <summary>
class Collider
{
public: // -- 公開 メンバ関数 -- //


	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void CreateRootSigneture() = 0;
	virtual void CreatePSO() = 0;
	virtual void CreateTransformation() = 0;
	virtual void CreateVertex() = 0;
	virtual void CreateIndex() = 0;

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

protected: // -- 限定公開 メンバ変数 -- //

	// 衝突属性 自分
	uint32_t mCollisionAttribute = 0xffffffff;
	// 衝突属性 相手
	uint32_t mCollisionMask = 0xffffffff;

	// -- ここから下はデバッグ用の変数 -- //

	// 外部ポインタ
	DirectXCommon* mDxCommon = nullptr;
	Matrix4x4 viewM, wvpM;

	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mWvpResource = nullptr;
	// データを書き込む
	TransformationMatrixForGrid3D* mWvpData = nullptr;
	WorldTransform* pWorldTransform;

	// 頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexResource = nullptr;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};
	// 頂点データ
	VertexDataForGrid* mVertexData = nullptr;

	// Indexリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> mIndexResource = nullptr;
	// Indexバッファビュー
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView{};

	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> mGraphicsPipelineState = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr <ID3D12RootSignature> mRootSignature = nullptr;
	
};