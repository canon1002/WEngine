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

	void Initialize(DirectXCommon* dxCommon,CameraCommon* camera);
	void Update();
	void Draw();

	void CreateVertexResource();
	void CreateIndexResource();
	void CreateTransformationRsource();
	void CreateBufferView();

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return vertexBufferView; }
	auto* GetMaterial() { return  materialResourceSprite.Get(); }
	auto* GetWVP() { return wvpResource.Get(); }
	void SetTexture(std::string filepath){
		textureHandle_ = dxCommon_->srv_->LoadTexture(filepath); 
		AdjustTextureSize();
	}
	void SetColor(Color color) { materialData->color = color; }
	void SetTextureSize(Vec2 size) { textureSize_ = size; }
	void SetSpriteSize(Vec2 size) { spriteSize = size; }
	void SetPos(Vec2 pos) {
		worldTransform_.translation.x = pos.x;
		worldTransform_.translation.y = pos.y;
	}
	void SetAnchorPoint(Vec2 point) { anchorPoint = point; }

private:

	// テクスチャサイズをイメージに合わせる
	void AdjustTextureSize();

private:

	// 外部ポインタ
	CameraCommon* camera_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

	WorldTransform worldTransform_;
	Mat44 worldM, cameraM, viewM, projectM, wvpM;
	Vec4 translate_;
	// テクスチャハンドル
	int32_t textureHandle_ = 0;

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	// マテリアル用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite = nullptr;
	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = nullptr;
	// Index用リソースデータの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = nullptr;
	// データを書き込む
	TransformationMatrix* wvpData = nullptr;
	// 頂点リソースにデータを書き込む
	VertexData2D* vertexData = nullptr;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//　インデックスはuint32_tとする
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	// マテリアルデータ(LightingはしないのでColorとuvTransformのみ)
	Material2D *materialData = nullptr;
	// UVTransform用の変数
	UVTransform uvTransform_;

	// アンカーポイント
	Vec2 anchorPoint = { 0.0f,0.0f };

	// スプライトのサイズ
	Vec2 spriteSize = { 256.0f,256.0f };

	// テクスチャ左上座標
	Vec2 textureLeftTop_ = { 0.0f,0.0f };
	// テクスチャ全体サイズ
	Vec2 textureFullSize_ = { 64.0f,64.0f };
	// テクスチャ切り出しサイズ
	Vec2 textureSize_ = { 64.0f,64.0f };

};
