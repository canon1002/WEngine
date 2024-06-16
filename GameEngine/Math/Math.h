#pragma once
#include"Vec2.h"
#include"Vector3.h"
#include"Vec4.h"
#include"Matrix4x4.h"
#include"Quaternion.h"

/// <summary>
/// 回転がVector3で表されるTransform構造体
/// </summary>
struct EulerTransform {
	Vector3 scale_;			// 拡大率
	Vector3 rotation_;		// 回転
	Vector3 translation_;	// 平行移動
};

/// <summary>
/// 回転がQuaternionで表されるTransform構造体
/// </summary>
struct QuaternionTransform {
	Vector3 scale_;			// 拡大率
	Quaternion rotation_;	// 回転
	Vector3 translation_;	// 平行移動

	// 親となるワールド変換へのポインタ（読み取り専用）
	QuaternionTransform* parent_ = nullptr;

	void Init() {
		scale_ = { 1.0f,1.0f,1.0f };
		rotation_ = { 0.0f,0.0f,0.0f };
		translation_ = { 0.0f,0.0f,0.0f };
		parent_ = nullptr;
	}

	Matrix4x4 GetAffinMatrix() const {
		Matrix4x4 result = MakeAffineMatrix(scale_, rotation_, translation_);

		// 親があれば親のワールド行列を掛ける
		if (parent_ != nullptr) {
			result = Multiply(result, parent_->GetAffinMatrix());
		}
		return result;
	}

	Vector3 GetWorldPosition() {
		// ワールド行列を生成
		Matrix4x4 worldMatrix = GetAffinMatrix();
		return Vector3(worldMatrix.m[3][0], worldMatrix.m[3][1], worldMatrix.m[3][2]);
	}

	void SetParent(QuaternionTransform* parent) {
		// 親のポインタをセット
		parent_ = parent;
	}
};