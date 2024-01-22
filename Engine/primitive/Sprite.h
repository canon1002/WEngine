#pragma once
#include "../math/Math.h"
#include "../base/DirectXCommon.h"
#include "../resources/Section/Resource.h"
#include "../object/worldTransform/WorldTransform.h"

class Sprite
{
public:
	Sprite();
	~Sprite();

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
	void SetTransform(Vec3 pos) {
		worldtransform_->translate = pos;
	}

	/// <summary>
	/// 中心座標を移動させる
	/// </summary>
	/// <param name="t">移動量</param>
	void Transform(Vec3 t) {
		worldtransform_->translate.x += t.x;
		worldtransform_->translate.y += t.y;
		worldtransform_->translate.z += t.z;
	}

	/// <summary>
	/// 回転させる
	/// </summary>
	/// <param name="r">回転量</param>
	void Rotation(Vec3 r) {
		worldtransform_->rotate.x += r.x;
		worldtransform_->rotate.y += r.y;
		worldtransform_->rotate.z += r.z;
	}

	/// <summary>
	/// 色変更
	/// </summary>
	/// <param name="color"></param>
	void SetColor(Color color) {
		// 指定した色に書き込む
		//*materialData = { Color(color.r, color.g, color.b, color.a) };
	}

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return vertexBufferView; }
	auto* GetMaterial() { return  materialResourceSprite.Get(); }
	auto* GetWVP() { return wvpResource.Get(); }

private:

	// 外部ポインタ
	MatrixCamera* mainCamera_ = nullptr;
	DirectXCommon* dx_ = nullptr;

	WorldTransform* worldtransform_ = nullptr;
	Mat44 worldM, cameraM, viewM, projectM, wvpM;
	Vec4 translate_;

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
	VertexData* vertexData = nullptr;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//　インデックスはuint32_tとする
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	// マテリアルデータ
	Material *materialData = nullptr;
	// UVTransform用の変数
	Transformation uvTransform_;
};

