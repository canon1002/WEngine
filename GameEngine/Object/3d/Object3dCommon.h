#pragma once
#include "Math.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Light/DirectionalLight.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Camera/CameraCommon.h"

class Object3dCommon
{
	void Init(DirectXCommon* dxCommon);
	void Update();
	void Draw();
	void SetCamera(CameraCommon* camera) { camera_ = camera; }

private:

	// 外部ポインタ
	CameraCommon* camera_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

	//WorldTransform* worldTransform_ = nullptr;
	//WorldTransform* cameraWorldTransform_ = nullptr;
	//Mat44 cameraM, viewM, projectM, pespectiveM, wvpM;
	//// 半径
	//float rad = 1.0f;

	//// VertexResourceを生成する(P.42)
	//// 実際に頂点リソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	//// マテリアル用のResourceを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
	//// Transformation用のResourceを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = nullptr;
	//// Light用のリソースデータを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = nullptr;
	//// データを書き込む
	//TransformationMatrix* wvpData = nullptr;
	//// 頂点リソースにデータを書き込む
	//VertexData* vertexData = nullptr;
	//// 頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//// マテリアルデータ
	//Material* materialData_ = nullptr;
	//// 平行光源　
	//DirectionalLight* directionalLightDate = nullptr;

	//// テクスチャ切り替え
	//bool useBall = true;
	//// テクスチャハンドル
	//int32_t textureHandle_;
	//// モデルデータ
	//ModelData modelData;
	//// UVTransform用の変数
	//Transformation uvTransform_;

};

