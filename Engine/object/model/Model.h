#pragma once
#include "../../math/Math.h"
#include "../../base/DirectXCommon.h"
#include "../worldTransform/WorldTransform.h"
#include "../../resources/Section/Resource.h"
#include "../../object/light/DirectionalLight.h"

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData{
	std::vector<VertexData> vertices;
	MaterialData material;
};

class Model
{
public: 

	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
	
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);


public:
	Model();
	~Model();

	void Initialize();
	void Update();
	void Draw();

	void CreateVertexResource();
	void CreateIndexResource();
	void CreateTransformationRsource();
	void CreateBufferView();


	/// <summary>
	///	座標変更
	/// </summary>
	/// <param name="pos">座標</param>
	void SetTransform(Vector3 pos) {
		worldTransform_->translate = pos;
	}

	/// <summary>
	/// 中心座標を移動させる
	/// </summary>
	/// <param name="t">移動量</param>
	void Transform(Vector3 t) {
		worldTransform_->translate.x += t.x;
		worldTransform_->translate.y += t.y;
		worldTransform_->translate.z += t.z;
	}

	/// <summary>
	/// 回転させる
	/// </summary>
	/// <param name="r">回転量</param>
	void Rotation(Vector3 r) {
		worldTransform_->rotate.x += r.x;
		worldTransform_->rotate.y += r.y;
		worldTransform_->rotate.z += r.z;
	}

	/// <summary>
	/// 色変更
	/// </summary>
	/// <param name="color"></param>
	void SetColor(Vector4 color) {
		// 指定した色に書き込む
		*materialData_ = { Vector4(color.x, color.y, color.z, color.w) };
	}

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return vertexBufferView; }
	auto* GetMaterial() { return  materialResource.Get(); }
	auto* GetWVP() { return wvpResource.Get(); }

private:

	// 外部ポインタ
	MatrixCamera* mainCamera_ = nullptr;
	DirectXCommon* dx_ = nullptr;

	WorldTransform* worldTransform_ = nullptr;
	WorldTransform* cameraWorldTransform_ = nullptr;
	Matrix4x4 cameraM, viewM, projectM, pespectiveM, wvpM;
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
	Material* materialData_ = nullptr;
	// 平行光源　
	DirectionalLight* directionalLightDate = nullptr;

	// テクスチャ切り替え
	bool useBall = true;
	// テクスチャハンドル
	int32_t textureHandle_;
	// モデルデータ
	ModelData modelData;
	// UVTransform用の変数
	Math::Transform uvTransform_;
};
