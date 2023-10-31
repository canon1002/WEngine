#pragma once
#include "../math/Math.h"
#include "../base/DirectXCommon.h"
#include "../resources/Section/Resource.h"
#include "../object/worldTransform/WorldTransform.h"

// 前方宣言
class MatrixCamera;

class Triangle
{
public:
	Triangle();
	~Triangle();

	void Initialize();
	void Update();
	void Draw();

	void CreateVertexResource();
	void CreateBufferView();

	void SetColor(Vector4 color) {
		// 指定した色に書き込む
		*materialDate = Vector4(color.x, color.y, color.z, color.w);
	}

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return vertexBufferView; }
	auto* GetMaterial() {return  materialResource.Get();}
	


private:

	// 外部ポインタ
	MatrixCamera* mainCamera_ = nullptr;
	DirectXCommon* dx_ = nullptr;

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	// マテリアル用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;

	// ワールドトランスフォーム
	WorldTransform* worldTransform = nullptr;

	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// マテリアルデータ
	Vector4* materialDate = nullptr;
};

