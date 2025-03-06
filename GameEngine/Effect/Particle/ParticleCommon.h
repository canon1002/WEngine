#pragma once
#include "GameEngine/Math/Math.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Object/light/DirectionalLight.h"
#include <random>
#include "GameEngine/Object/Model/Model.h"

class CameraCommon;

// パーティクル1つの要素
struct Particle {
	WorldTransform worldTransform;
	Vector3 vel;
	Color color;
	float lifeTime;
	float currentTime;
};

// エミッター(パーティクル発生装置)
struct Emitter {
	std::unique_ptr<WorldTransform> worldtransform;// エミッターの座標
	uint32_t count;// 発生数
	float frequency;// 発生頻度
	float frequencyTime;// 頻度用時刻
};

/// <summary>
/// パーティクル基底クラス
/// </summary>
class ParticleCommon{

public: // -- 公開 メンバ変数 -- //

	virtual void Init();
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual Particle Create(const Vector3& translate, std::mt19937& randomEngine) = 0;
	virtual std::list<Particle> Emit(const Emitter& emtter,std::mt19937& randomEngine) = 0;

	// 実行状況切り替え
	virtual void SetActive(bool flag) { mIsActive = flag; }
	// 実行状況の取得
	virtual bool GetActive() { return mIsActive; }

	// エミッターの座標変更
	inline void SetEmitterWorldTransform(const Vector3& translation) { mEmitter.worldtransform->translation = translation; }

protected: // -- 限定公開 メンバ関数 -- //

	virtual void CreateTransformation();
	virtual void CreateVertex();
	virtual void CreateIndex();
	virtual void CreateMaterial();
	virtual void CreateInstancing();

protected: // -- 限定公開 メンバ変数 -- //

	// パーティクルのリスト
	std::list<Particle> mParticles;
	// エミッター
	Emitter mEmitter;

	WorldTransform mWorldTransform;

	// 半径
	float rad = 1.0f;

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexResource = nullptr;
	// マテリアル用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mMaterialResource = nullptr;
	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mWVPResource = nullptr;
	// TransformationMatrixを10コ格納できるResourceを作成する
	Microsoft::WRL::ComPtr<ID3D12Resource> mInstancingResource = nullptr;
	// データを書き込む
	ParticleForGPU* mWVPData = nullptr;
	ParticleForGPU* mInstancingData = nullptr;
	// 頂点リソースにデータを書き込む
	VertexData* mVertexData = nullptr;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};
	// マテリアルデータ
	Material* mMaterialData = nullptr;

	// テクスチャハンドル
	int32_t mTextureHandle;
	int32_t mInstancingHandle;

	// モデルデータ
	ModelData mModelData;
	// UVTransform用の変数
	UVTransform mUVTransform;

	// インスタンスの数
	const int32_t kNumMaxInstance = 400;
	int32_t mInstanceCount;

	// 乱数生成機
	std::random_device mSeedGenerator;
	std::mt19937 mRandomEngine;

	// 発生させるか
	bool mIsActive;

	// ビルボードの有効化
	bool mIsUseBillboard = true;

};

