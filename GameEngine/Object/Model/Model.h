#pragma once
#include "GameEngine/Math/Math.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Light/DirectionalLight.h"
#include "GameEngine/Object/Camera/CameraCommon.h"

class DirectXCommon;

class Model
{
public:

	~Model();
	void Initialize(DirectXCommon* dxCommon,CameraCommon* camera, const std::string& directrypath, const std::string& filename);
	void Update();
	void Draw();

	void CreateVertexResource();
	void CreateMaterialResource();

	void SetCameraPosition(CameraCommon camera) { cameraData->worldPosition = camera.GetTranslate(); }

private:

	// ModelCommonのポインタ
	//ModelCommon* modelCommon_ = nullptr;
	// DirectXのポインタ
	DirectXCommon* dxCommon_ = nullptr;
	CameraCommon* camera_ = nullptr;

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

public:

	// マテリアルデータ
	Material* materialData_ = nullptr;

private:


	// Light用のリソースデータを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = nullptr;
	// 平行光源　
	DirectionalLight* directionalLightDate = nullptr;

	// 鏡面反射用のリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> CameraResource = nullptr;
	CameraForGPU* cameraData = nullptr;

	// UVTransform用の変数
	UVTransform uvTransform_;

};
