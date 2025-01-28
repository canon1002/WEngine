#pragma once
#include "GameEngine/Math/Math.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Object/Camera/MainCamera.h"

class Sprite
{
public:
	Sprite() = default;
	~Sprite() = default;

	void Init();
	void Update();
	void Draw();

	void CreateVertexResource();
	void CreateIndexResource();
	void CreateTransformation();
	void CreateBufferView();

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return mVertexBufferView; }
	auto* GetMaterial() { return  materialResourceSprite.Get(); }
	auto* GetWVP() { return mWvpResource.Get(); }
	void SetTexture(std::string filepath){
		mTextureHandle = DirectXCommon::GetInstance()->mSrv->LoadTexture(filepath);
		AdjustTextureSize();
	}
	void SetTexture(int32_t textureHandle) {
		mTextureHandle = textureHandle;
		AdjustTextureSize();
	}

	void SetColor(Color color) { materialData->color = color; }
	void SetAlpha(float alpha) { materialData->color.a = alpha; }
	void SetTextureSize(Vector2 size) { textureSize_ = size; }
	void SetSpriteSize(Vector2 size) { spriteSize = size; }
	void SetPos(Vector2 pos) {
		mWorldTransform.translation.x = pos.x;
		mWorldTransform.translation.y = pos.y;
	}
	void SetAnchorPoint(Vector2 point) { anchorPoint = point; }
	void SetScale(Vector2 scale) { 
		mWorldTransform.scale.x = scale.x;
		mWorldTransform.scale.y = scale.y;
	}

	Vector2 GetScale() { return Vector2(mWorldTransform.scale.x, mWorldTransform.scale.y); }
	Vector2 GetPos() { return Vector2(mWorldTransform.translation.x, mWorldTransform.translation.y); }


private:

	// テクスチャサイズをイメージに合わせる
	void AdjustTextureSize();

private:

	// 外部ポインタ
	CameraCommon* mCamera = nullptr;
	

	WorldTransform mWorldTransform;
	Matrix4x4 worldM, cameraM, viewM, projectM, wvpM;
	Vector4 translate_;
	// テクスチャハンドル
	int32_t mTextureHandle = 0;

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexResource = nullptr;
	// マテリアル用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite = nullptr;
	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mWvpResource = nullptr;
	// Index用リソースデータの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = nullptr;
	// データを書き込む
	TransformationMatrix* mWvpData = nullptr;
	// 頂点リソースにデータを書き込む
	VertexData2D* mVertexData = nullptr;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};
	//　インデックスはuint32_tとする
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	// マテリアルデータ(LightingはしないのでColorとuvTransformのみ)
	Material2D *materialData = nullptr;
	// UVTransform用の変数
	UVTransform uvTransform_;

	// アンカーポイント
	Vector2 anchorPoint = { 0.0f,0.0f };

	// スプライトのサイズ
	Vector2 spriteSize = { 256.0f,256.0f };

	// テクスチャ左上座標
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	// テクスチャ全体サイズ
	Vector2 textureFullSize_ = { 64.0f,64.0f };
	// テクスチャ切り出しサイズ
	Vector2 textureSize_ = { 64.0f,64.0f };

};

