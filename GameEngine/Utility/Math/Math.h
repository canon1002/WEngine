#pragma once
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"
#include"Matrix4x4.h"
#include"Quaternion.h"

/// <summary>
/// 回転がVector3で表されるTransform構造体
/// </summary>
struct EulerTransform {
	Vector3 scale;			// 拡大率
	Vector3 rotation;		// 回転
	Vector3 translation;	// 平行移動
};

/// <summary>
/// 回転がQuaternionで表されるTransform構造体
/// </summary>
struct QuaternionTransform {
	Vector3 scale;			// 拡大率
	Quaternion rotation;	// 回転
	Vector3 translation;	// 平行移動

	// 親となるワールド変換へのポインタ（読み取り専用）
	QuaternionTransform* parent = nullptr;

	void Init() {
		scale = { 1.0f,1.0f,1.0f };
		rotation = { 0.0f,0.0f,0.0f };
		translation = { 0.0f,0.0f,0.0f };
		parent = nullptr;
	}

	Matrix4x4 GetAffinMatrix() const {
		Matrix4x4 result = MakeAffineMatrix(scale, rotation, translation);

		// 親があれば親のワールド行列を掛ける
		if (parent != nullptr) {
			result = Multiply(result, parent->GetAffinMatrix());
		}
		return result;
	}

	Vector3 GetWorldPosition() {
		// ワールド行列を生成
		Matrix4x4 worldMatrix = GetAffinMatrix();
		return Vector3(worldMatrix.m[3][0], worldMatrix.m[3][1], worldMatrix.m[3][2]);
	}

	void SetParent(QuaternionTransform* p) {
		// 親のポインタをセット
		parent = p;
	}
};
