#pragma once
#include "Math.h"
#include "../worldTransform/WorldTransform.h"
#include "../../resources/Section/Resource.h"
#include "../../object/light/DirectionalLight.h"

class DirectXCommon;
class ModelCommon;


class Model
{
public:

	void Initialize(const std::string& directrypath, const std::string& filename);
	void Update();
	void Draw();

	void CreateVertexResource();
	void CreateMaterialResource();



private:

	// ModelCommonのポインタ
	ModelCommon* modelCommon_ = nullptr;
	// DirectXのポインタ
	DirectXCommon* dx_ = nullptr;

	// モデルデータ
	ModelData modelData;
	// テクスチャハンドル
	int32_t textureHandle_;


	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;


	// マテリアル用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
	// マテリアルデータ
	Material* materialData_ = nullptr;


	// Light用のリソースデータを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = nullptr;
	// 平行光源　
	DirectionalLight* directionalLightDate = nullptr;


	// UVTransform用の変数
	Transformation uvTransform_;

};

