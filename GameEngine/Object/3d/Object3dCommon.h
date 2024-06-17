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
	void SetCamera(CameraCommon* camera) { mCamera = camera; }

private:

	// 外部ポインタ
	CameraCommon* mCamera = nullptr;
	DirectXCommon* mDxCommon = nullptr;

	//WorldTransform* mWorldTransform = nullptr;
	//WorldTransform* cameraWorldTransform_ = nullptr;
	//Matrix4x4 cameraM, viewM, projectM, pespectiveM, wvpM;
	//// 半径
	//float rad = 1.0f;

	//// VertexResourceを生成する(P.42)
	//// 実際に頂点リソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> mVertexResource = nullptr;
	//// マテリアル用のResourceを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
	//// Transformation用のResourceを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> mWvpResource = nullptr;
	//// Light用のリソースデータを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = nullptr;
	//// データを書き込む
	//TransformationMatrix* mWvpData = nullptr;
	//// 頂点リソースにデータを書き込む
	//VertexData* mVertexData = nullptr;
	//// 頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};
	//// マテリアルデータ
	//Material* materialData_ = nullptr;
	//// 平行光源　
	//DirectionalLight* directionalLightDate = nullptr;

	//// テクスチャ切り替え
	//bool useBall = true;
	//// テクスチャハンドル
	//int32_t mTextureHandle;
	//// モデルデータ
	//ModelData modelData;
	//// UVTransform用の変数
	//Transformation uvTransform_;

};

