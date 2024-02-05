#pragma once
#include "../math/Math.h"
#include "../base/DirectXCommon.h"
#include "../resources/Section/Resource.h"
#include "../object/worldTransform/WorldTransform.h"

// 前方宣言
class MainCamera;

class Triangle
{
public:
	Triangle();
	~Triangle();

	void Initialize();
	void Update();
	void Draw();

	void CreateVertexResource();
	void CreateTransformationRsource();
	void CreateBufferView();
	void DrawBeginResource();

	/// <summary>
	///	座標変更
	/// </summary>
	/// <param name="pos">座標</param>
	void SetTransform(Vec3 pos) { 
		worldTransform_.translate = pos;
	}
	
	/// <summary>
	/// 中心座標を移動させる
	/// </summary>
	/// <param name="t">移動量</param>
	void Transform(Vec3 t) { 
		worldTransform_.translate.x += t.x;
		worldTransform_.translate.y += t.y;
		worldTransform_.translate.z += t.z;
	}

	/// <summary>
	/// 回転させる
	/// </summary>
	/// <param name="r">回転量</param>
	void Rotation(Vec3 r) {
		worldTransform_.rotate.x += r.x;
		worldTransform_.rotate.y += r.y;
		worldTransform_.rotate.z += r.z;
	}

	/// <summary>
	/// 色変更
	/// </summary>
	/// <param name="color"></param>
	void SetColor(Color color) {
		// 指定した色に書き込む
		*materialData = Color(color.r, color.g, color.b, color.a);
	}

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return vertexBufferView; }
	auto* GetMaterial() {return  materialResource.Get();}
	auto* GetWVP() {return wvpResource.Get();}

private:

	// 外部ポインタ
	MainCamera* mainCamera_ = nullptr;
	DirectXCommon* dx_ = nullptr;

	WorldTransform worldTransform_;
	Mat44 worldM, cameraM, viewM, projectM, pespectiveM,wvpM;
	Vec4 translate_;

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	// マテリアル用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = nullptr;
	// データを書き込む
	Mat44* wvpData = nullptr;
	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// マテリアルデータ
	Color* materialData = nullptr;
};

