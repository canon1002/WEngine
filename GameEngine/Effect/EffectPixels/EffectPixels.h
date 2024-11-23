#pragma once
#include "GameEngine/Math/Math.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Model/Model.h"

class EffectPixels
{
public:
	EffectPixels() = default;
	~EffectPixels() = default;

	void Init();
	void Update();
	void Draw();
	void DrawGui(std::string name);

	void CreateVertexResource();
	void CreateTransformation();
	void CreateMaterial();

	void SetVertices(Vector3 beforeTop,Vector3 afterTop,Vector3 beforeButtom,Vector3 afterButtom);
	Vector4 GetVertices(const int index) { return mVertexData[index].position; }

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return mVertexBufferView; }
	auto* GetMaterial() { return  mMaterialResource.Get(); }
	auto* GetWVP() { return mWvpResource.Get(); }

	void SetColor(Color color) { mMaterialData->color = color; }
	Color GetColor() {return mMaterialData->color;}

	// 生存判定
	bool GetIsActive()const;


	// 生存時間
	float mActiveCount;
	float mActiveCountMax;


private:

	// モデルデータ
	ModelData mModelData;

	// 外部ポインタ
	CameraCommon* mCamera = nullptr;
	DirectXCommon* mDxCommon = nullptr;

	// ワールド座標
	WorldTransform* mWorldTransform;
	
	// テクスチャハンドル
	int32_t mTextureHandle = 0;
	int32_t mTextureHandleCubeMap;


	//std::array<Vector3, 4> mVertex;

	// -- Vertex -- //
	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexResource = nullptr;
	VertexData* mVertexData = nullptr;
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};


	// -- Material -- //
	Microsoft::WRL::ComPtr<ID3D12Resource> mMaterialResource = nullptr;
	Material* mMaterialData = nullptr;


	// -- Transfomation -- //
	Microsoft::WRL::ComPtr<ID3D12Resource> mWvpResource = nullptr;
	TransformationMatrix* mWvpData = nullptr;


	// -- Index -- //
	Microsoft::WRL::ComPtr<ID3D12Resource> mIndexResource = nullptr;
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView{};


	// -- Light -- //

	Microsoft::WRL::ComPtr<ID3D12Resource> mDirectionalLightResource = nullptr;
	DirectionalLight* mDirectionalLightData = nullptr; // 平行光源　
	bool mIsLighting = true;

	// -- 鏡面反射 -- //

	// -- カメラ -- // 
	Microsoft::WRL::ComPtr<ID3D12Resource> mCameraResource = nullptr;
	CameraForGPU* mCameraData = nullptr;
	
	// UVTransform用の変数
	UVTransform mUVTransform;
	

};

