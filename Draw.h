#pragma once
#include "DirectX.h"

// 前方宣言
class MatrixCamera;
struct Vector4;
struct Matrix4x4;

class Draw
{
private:

	Draw();
	~Draw();

public:

	// コピーコンストラクタと演算子オーバーロードの禁止
	Draw(const Draw& obj) = delete;
	Draw& operator=(const Draw& obj) = delete;
	// インスタンスを取得
	static Draw* GetInstance();

	void Initialize(WinAPI* win, DirectX* dx, MatrixCamera* mainCamera);
	void Delete();

	void CreateVertexResource();
	void CreateTransformationRsource();
	void CreateBufferView();

	void DrawBegin();

	void DrawEnd();

public:

	// ポインタで受け取る
	WinAPI* win_ = nullptr;
	DirectX* dx_ = nullptr;
	MatrixCamera* mainCamera_;

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	ID3D12Resource* vertexResource = nullptr;

	

	// マテリアル用のResourceを作る
	ID3D12Resource* materialResource = nullptr;
	// Transformation用のResourceを作る
	ID3D12Resource* wvpResource = nullptr;
	// データを書き込む
	Matrix4x4* wvpData = nullptr;
	// 頂点リソースにデータを書き込む
	Vector4* vertexData = nullptr;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

};
