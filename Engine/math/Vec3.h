#pragma once

struct Vec3 {
	float x, y, z;

};
Vec3 operator+(Vec3 obj);
Vec3& operator+=(Vec3& a, const Vec3& b);
Vec3 operator-(Vec3 obj);
Vec3 operator*(Vec3 obj, float f);


Vec3 Add(const Vec3& v1, const Vec3& v2);

Vec3 Subtract(const Vec3& v1, const Vec3& v2);

Vec3 Scalar(float scalar, const Vec3& v);

Vec3 Leap(const Vec3& p0, const Vec3& p1, const float t);

/// <summary>
/// ベクトルの長さを求める
/// </summary>
/// <param name="v"></param>
/// <returns></returns>
static float Length(const Vec3& v);

/// <summary>
/// 正規化
/// </summary>
/// <param name="v"></param>
/// <returns></returns>
Vec3 Nomalize(const Vec3& v);

/// <summary>
/// クロス積・外積
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
Vec3 Cross(const Vec3& v1, const Vec3& v2);

/// <summary>
/// ドット積・内積
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
float Dot(const Vec3& v1, const Vec3& v2);

/// <summary>
/// 正射影ベクトル
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
Vec3 Project(const Vec3& v1, const Vec3& v2);

/// <summary>
/// 最近接点
/// </summary>
/// <param name="point"></param>
/// <param name="segment"></param>
/// <returns></returns>
//Vec3 ClosestPoint(const Vec3& point, const Segment& segment);

/// <summary>
/// 
/// </summary>
/// <param name="v"></param>
/// <returns></returns>
Vec3 Perpendicular(const Vec3& v);

