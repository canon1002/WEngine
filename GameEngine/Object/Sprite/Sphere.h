#pragma once
#include "GameEngine/Math/Math.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Object/light/DirectionalLight.h"

// 前方宣言
class CameraCommon;

class Sphere
{
public:
	Sphere();
	~Sphere();

	void Initialize(DirectXCommon* dxCommon,CameraCommon* camera);
	void Update();
	void Draw() const;

	void CreateVertexResource();
	void CreateTransformationRsource();
	void CreateBufferView();
	void DrawBeginResource();

	/// <summary>
	///	座標変更
	/// </summary>
	/// <param name="pos">座標</param>
	void SetTransform(Vec3 pos) {
		worldTransform_.translation = pos;
	}

	/// <summary>
	/// 中心座標を移動させる
	/// </summary>
	/// <param name="t">移動量</param>
	void Transform(Vec3 t) {
		worldTransform_.translation.x += t.x;
		worldTransform_.translation.y += t.y;
		worldTransform_.translation.z += t.z;
	}

	/// <summary>
	/// 回転させる
	/// </summary>
	/// <param name="r">回転量</param>
	void Rotation(Vec3 r) {
		worldTransform_.rotation.x += r.x;
		worldTransform_.rotation.y += r.y;
		worldTransform_.rotation.z += r.z;
	}

	/// <summary>
	/// 色変更
	/// </summary>
	/// <param name="color"></param>
	void SetColor(Color color) {
		// 指定した色に書き込む
		materialData->color = Color(color.r, color.g, color.b, color.a);
	}

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return vertexBufferView; }
	auto* GetMaterial() { return  materialResource.Get(); }
	auto* GetWVP() { return wvpResource.Get(); }

private:

	// 外部ポインタ
	CameraCommon* camera_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

	WorldTransform worldTransform_;
	Mat44 cameraM, viewM, projectM, pespectiveM, wvpM;
	// 半径
	float rad = 1.0f;

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	// マテリアル用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = nullptr;
	// Light用のリソースデータを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = nullptr;
	// データを書き込む
	TransformationMatrix* wvpData = nullptr;
	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// マテリアルデータ
	Material* materialData = nullptr;
	// 平行光源　
	DirectionalLight* directionalLightDate = nullptr;

	// テクスチャ切り替え
	bool useBall = true;

private:

	// 円周率
	const float pi = 3.14f;
	// 球体の分割数
	const uint32_t kSubdivision = 16;
	// 緯度
	const float kLonEvery = (2.0f * pi) / (float)kSubdivision;
	// 経度
	const float kLatEvery = pi / (float)kSubdivision;

};

