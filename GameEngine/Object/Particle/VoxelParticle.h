#pragma once
#include "GameEngine/Math/Math.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Object/light/DirectionalLight.h"
#include <random>
#include "GameEngine/Object/Model/Model.h"

class CameraCommon;

struct Particle {
	Vector3 scale;
	Vector3 rotation;
	Vector3 translation;
	Vector3 vel;
	Color color;
	float lifeTime;
	float currentTime;
};

class VoxelParticle
{
public:
	VoxelParticle();
	~VoxelParticle();

	void Initialize(DirectXCommon* dxCommon,CameraCommon* camera);
	void Update();
	void Draw();

	void CreateVertexResource();
	void CreateIndexResource();
	void CreateTransformationRsource();
	void CreateBufferView();

	Particle MakeNewParticle(std::mt19937& randomEngine);

	/// <summary>
	///	座標変更
	/// </summary>
	/// <param name="pos">座標</param>
	void SetTransform(Vector3 pos) {
		mWorldTransform.translation = pos;
	}

	/// <summary>
	/// 中心座標を移動させる
	/// </summary>
	/// <param name="t">移動量</param>
	void Transform(Vector4 t) {
		mWorldTransform.translation.x += t.x;
		mWorldTransform.translation.y += t.y;
		mWorldTransform.translation.z += t.z;
	}

	/// <summary>
	/// 回転させる
	/// </summary>
	/// <param name="r">回転量</param>
	void Rotation(Vector3 r) {
		mWorldTransform.rotation.x += r.x;
		mWorldTransform.rotation.y += r.y;
		mWorldTransform.rotation.z += r.z;
	}

	/// <summary>
	/// 色変更
	/// </summary>
	/// <param name="color"></param>
	void SetColor(Color color) {
		// 指定した色に書き込む
		*materialData_ = { Color(color.r, color.g, color.b, color.a) };
	}

	void SetTexture(int32_t textureId) {
		textureHandle_ = textureId;
	}

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return vertexBufferView; }
	auto* GetMaterial() { return  materialResource.Get(); }
	auto* GetWVP() { return mWvpResource.Get(); }

private:

	// 外部ポインタ
	CameraCommon* camera_ = nullptr;
	DirectXCommon* mDxCommon = nullptr;

	WorldTransform mWorldTransform;
	Matrix4x4 cameraM, viewM, projectM, pespectiveM, wvpM;
	// 半径
	float rad = 1.0f;

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	// マテリアル用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mWvpResource = nullptr;
	// Light用のリソースデータを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = nullptr;
	// TransformationMatrixを10コ格納できるResourceを作成する
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource = nullptr;
	// データを書き込む
	ParticleForGPU* mWvpData = nullptr;
	ParticleForGPU* instancingData_ = nullptr;
	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// マテリアルデータ
	Material* materialData_ = nullptr;
	// 平行光源　
	DirectionalLight* directionalLightDate = nullptr;

	// テクスチャ切り替え
	bool useBall = true;
	// テクスチャハンドル
	int32_t textureHandle_;
	int32_t instancingHandle_;
	// モデルデータ
	ModelData modelData_;
	// UVTransform用の変数
	UVTransform uvTransform_;
	// インスタンスの数
	const int32_t kNumMaxInstance = 6;
	int32_t instanceCount_;

	// パーティクル
	Particle particles[10];
	
	// 乱数生成機
	std::random_device seedGenerator_;
	std::mt19937 randomEngine_;

};

