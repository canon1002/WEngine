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

	Matrix4x4 GetAffinMatrix() const {
		Matrix4x4 result = MakeAffineMatrix(scale_, rotation_, translation_);

		// 親があれば親のワールド行列を掛ける
		if (parent_ != nullptr) {
			result = Multiply(result, parent_->GetAffinMatrix());
		}
		return result;
	}

};