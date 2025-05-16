#pragma once
#include "GameEngine/Utility/Math/Math.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Utility/Transform/WorldTransform.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Resource/Texture/TextureManager.h"

class Sprite
{
public: // -- 公開 メンバ関数 -- //

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
	auto* GetMaterial() { return  mMaterialResourceSprite.Get(); }
	auto* GetWVP() { return mWvpResource.Get(); }
	void SetTexture(std::string filepath){
		mTextureHandle = TextureManager::GetInstance()->LoadTexture(filepath);
		AdjustTextureSize();
	}
	void SetTexture(int32_t textureHandle) {
		mTextureHandle = textureHandle;
		AdjustTextureSize();
	}

	void SetColor(Color color) { mMaterialData->color = color; }
	void SetAlpha(float alpha) { mMaterialData->color.a = alpha; }
	void SetTextureSize(Vector2 size) { mTextureSize = size; }
	void SetSpriteSize(Vector2 size) { mSpriteSize = size; }
	void SetPos(Vector2 pos) {
		mWorldTransform.translation.x = pos.x;
		mWorldTransform.translation.y = pos.y;
	}
	void SetAnchorPoint(Vector2 point) { mAnchorPoint = point; }
	void SetScale(Vector2 scale) { 
		mWorldTransform.scale.x = scale.x;
		mWorldTransform.scale.y = scale.y;
	}

	Vector2 GetScale() { return Vector2(mWorldTransform.scale.x, mWorldTransform.scale.y); }
	Vector2 GetPos() { return Vector2(mWorldTransform.translation.x, mWorldTransform.translation.y); }


private: // -- 非公開 メンバ関数 -- //

	// テクスチャサイズをイメージに合わせる
	void AdjustTextureSize();

private: // -- 非公開 メンバ変数 -- //

	// 外部ポインタ
	CameraCommon* mCamera = nullptr;
	
	// ワールド座標
	WorldTransform mWorldTransform;
	Matrix4x4 worldM, cameraM, viewM, projectM, wvpM;
	Vector4 mTranslate;
	// テクスチャハンドル
	int32_t mTextureHandle = 0;

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexResource = nullptr;
	// マテリアル用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mMaterialResourceSprite = nullptr;
	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mWvpResource = nullptr;
	// Index用リソースデータの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> mIndexResource = nullptr;
	// データを書き込む
	TransformationMatrix* mWvpData = nullptr;
	// 頂点リソースにデータを書き込む
	VertexData2D* mVertexData = nullptr;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};
	//　インデックスはuint32_tとする
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView{};
	// マテリアルデータ(LightingはしないのでColorとuvTransformのみ)
	Material2D *mMaterialData = nullptr;
	// UVTransform用の変数
	UVTransform mUVTransform;

	// アンカーポイント
	Vector2 mAnchorPoint = { 0.0f,0.0f };
	// スプライトのサイズ
	Vector2 mSpriteSize = { 256.0f,256.0f };
	// テクスチャ左上座標
	Vector2 mTextureLeftTop = { 0.0f,0.0f };
	// テクスチャ全体サイズ
	Vector2 mTextureFullSize = { 64.0f,64.0f };
	// テクスチャ切り出しサイズ
	Vector2 mTextureSize = { 64.0f,64.0f };

};

