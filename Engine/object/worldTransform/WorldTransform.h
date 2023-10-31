#pragma once
#include "../../math/Math.h"
#include "../../base/DirectXCommon.h"

class MatrixCamera;

class WorldTransform
{
public:

	// 拡大縮小
	Vector3 scale_;
	// 回転
	Vector3 rotate_;
	// 平行移動
	Vector3 translate_; 

	// コンストラクタ
	WorldTransform();
	WorldTransform(Vector3 t);
	WorldTransform(Vector3 r, Vector3 t);
	WorldTransform(Vector3 s, Vector3 r, Vector3 t);

	// デストラクタ
	~WorldTransform();

	void UpdateMatrix(MatrixCamera& mainCamera);

	/// <summary>
	/// 定数バッファの取得
	/// </summary>
	/// <returns>定数バッファ</returns>
	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetConstBuffer() const { return constBuffer_; }

	void CreateTransformationResource(DirectXCommon& dx);

	auto* GetWVP() { return constBuffer_.Get(); }

private:

	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
	// マッピング済みアドレス
	Matrix4x4* constMap = nullptr;
};

