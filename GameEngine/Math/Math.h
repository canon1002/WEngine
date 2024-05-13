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
};
