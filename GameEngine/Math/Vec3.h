#pragma once
#include <vector>

struct Vec3 {
	float x, y, z;

};

//グローバル算術演算子のオーバーロード
Vec3 operator+(const Vec3& num1, const Vec3& num2);
Vec3 operator-(const Vec3& num1, const Vec3& num2);
Vec3 operator*(const Vec3& num1, const float num2);
Vec3 operator*(const float num1, const Vec3& num2);
Vec3 operator*(const Vec3& num1, const Vec3& num2);
Vec3 operator/(const Vec3& num1, const float num2);
Vec3 operator+=(Vec3& num1, const Vec3& num2);
Vec3 operator-=(Vec3& num1, const Vec3& num2);
Vec3 operator*=(Vec3& num1, const float num2);
Vec3 operator*=(Vec3& num1, const Vec3& num2);
Vec3 operator/=(Vec3& num1, const Vec3& num2);
Vec3 operator/=(Vec3& num1, const float num2);
bool operator==(const Vec3& num1, const Vec3& num2);



Vec3 Add(const Vec3& v1, const Vec3& v2);

Vec3 Subtract(const Vec3& v1, const Vec3& v2);

Vec3 Scalar(float scalar, const Vec3& v);

Vec3 Lerp(const Vec3& p0, const Vec3& p1, const float t);
Vec3 Slerp(const Vec3& p0, const Vec3& p1, const float t);

/// <summary>
/// CatmullRom補間
/// </summary>
/// <param name="p0">点0の座標</param>
/// <param name="p1">点0の座標</param>
/// <param name="p2">点0の座標</param>
/// <param name="p3">点0の座標</param>
/// <param name="t">点1を0.0f、点2を1.0fとした割合指定</param>
/// <returns>点1と点2の間で指定された座標</returns>
Vec3 CatmullRomInterpolation(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, const float t);

/// <summary>
/// CatmullRomスプライン曲線上の制御点を得る
/// </summary>
/// <param name="points"></param>
/// <param name="t"></param>
/// <returns></returns>
Vec3 CatmullRomPosition(const std::vector<Vec3>& points, float t);

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
Vec3 Normalize(const Vec3& v);

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

