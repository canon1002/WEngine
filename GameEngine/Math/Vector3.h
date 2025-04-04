#pragma once
#include <vector>

struct Vector3 {
	float x, y, z;

};

//グローバル算術演算子のオーバーロード
Vector3 operator+(const Vector3& num1, const Vector3& num2);
Vector3 operator-(const Vector3& num1, const Vector3& num2);
Vector3 operator*(const Vector3& num1, const float num2);
Vector3 operator*(const float num1, const Vector3& num2);
Vector3 operator*(const Vector3& num1, const Vector3& num2);
Vector3 operator/(const Vector3& num1, const float num2);
Vector3 operator+=(Vector3& num1, const Vector3& num2);
Vector3 operator-=(Vector3& num1, const Vector3& num2);
Vector3 operator*=(Vector3& num1, const float num2);
Vector3 operator*=(Vector3& num1, const Vector3& num2);
Vector3 operator/=(Vector3& num1, const Vector3& num2);
Vector3 operator/=(Vector3& num1, const float num2);
bool operator==(const Vector3& num1, const Vector3& num2);



Vector3 Add(const Vector3& v1, const Vector3& v2);

Vector3 Subtract(const Vector3& v1, const Vector3& v2);

Vector3 Scalar(float scalar, const Vector3& v);

float Lerp(const float& p0, const float& p1, const float t);
Vector3 Lerp(const Vector3& p0, const Vector3& p1, const float t);
Vector3 Slerp(const Vector3& p0, const Vector3& p1, const float t);

/// <summary>
/// 最短角度補間
/// </summary>
/// <param name="s"></param>
/// <param name="e"></param>
/// <param name="t"></param>
/// <returns></returns>
float LerpShortAngle(float s, float e, float t);

/// <summary>
/// CatmullRom補間
/// </summary>
/// <param name="p0">点0の座標</param>
/// <param name="p1">点0の座標</param>
/// <param name="p2">点0の座標</param>
/// <param name="p3">点0の座標</param>
/// <param name="t">点1を0.0f、点2を1.0fとした割合指定</param>
/// <returns>点1と点2の間で指定された座標</returns>
Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, const float t);

/// <summary>
/// CatmullRomスプライン曲線上の制御点を得る
/// </summary>
/// <param name="points"></param>
/// <param name="t"></param>
/// <returns></returns>
Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t);

/// <summary>
/// ベクトルの長さを求める
/// </summary>
/// <param name="v"></param>
/// <returns></returns>
float Length(const Vector3& v);

/// <summary>
/// 正規化
/// </summary>
/// <param name="v"></param>
/// <returns></returns>
Vector3 Normalize(const Vector3& v);

/// <summary>
/// クロス積・外積
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
Vector3 Cross(const Vector3& v1, const Vector3& v2);

/// <summary>
/// ドット積・内積
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
float Dot(const Vector3& v1, const Vector3& v2);

/// <summary>
/// 正射影ベクトル
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
Vector3 Project(const Vector3& v1, const Vector3& v2);

/// <summary>
/// 最近接点
/// </summary>
/// <param name="point">線分との距離を求める点</param>
/// <param name="origin">線分の始点</param>
/// <param name="diff">線分の始点から終点までの距離</param>
/// <returns></returns>
Vector3 ClosestPoint(const Vector3& point, const Vector3& origin,const Vector3& diff);

/// <summary>
/// 
/// </summary>
/// <param name="v"></param>
/// <returns></returns>
Vector3 Perpendicular(const Vector3& v);


Vector3 Min(const Vector3& a, const Vector3& b);
Vector3 Max(const Vector3& a, const Vector3& b);

Vector3 ExponentialInterpolation(Vector3 a, Vector3 b, float t,float k);
float ExponentialInterpolation(float a, float b, float t,float k);

float clamp(float num, float max, float min);
