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
	WorldTransform worldTransform;// エミッターの座標
	uint32_t count;// 発生数
	float frequency;// 発生頻度
	float frequencyTime;// 頻度用時刻
};

/// <summary>
/// パーティクル基底クラス
/// </summary>
class ParticleCommon{

public: // -- 公開 メンバ変数 -- //

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual Particle Create(std::mt19937& randomEngine) = 0;
	virtual std::list<Particle> Emit(const Emitter& emtter,std::mt19937& randomEngine) = 0;

protected: // -- 限定公開 メンバ関数 -- //

	virtual void CreateTransformation() = 0;
	virtual void CreateVertex() = 0;
	virtual void CreateIndex() = 0;
	virtual void CreateMaterial() = 0;
	virtual void CreateInstancing() = 0;

protected: // -- 限定公開 メンバ変数 -- //

	// 外部ポインタ
	CameraCommon* mCamera = nullptr;
	DirectXCommon* mDxCommon = nullptr;

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
	ModelData modelData_;
	// UVTransform用の変数
	UVTransform uvTransform_;

	// インスタンスの数
	const int32_t kNumMaxInstance = 100;
	int32_t instanceCount_;

	// 乱数生成機
	std::random_device seedGenerator_;
	std::mt19937 randomEngine_;


};
