#include "Vector3.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <functional>
#include <cassert>


#pragma region グローバル算術演算子のオーバーロード

Vector3 operator+(const Vector3& num1, const Vector3& num2) {
	Vector3 num3;

	num3.x = num1.x + num2.x;
	num3.y = num1.y + num2.y;
	num3.z = num1.z + num2.z;

	return num3;
}
Vector3 operator-(const Vector3& num1, const Vector3& num2) {
	Vector3 num3;

	num3.x = num1.x - num2.x;
	num3.y = num1.y - num2.y;
	num3.z = num1.z - num2.z;

	return num3;
}
Vector3 operator*(const Vector3& num1, const float num2) {
	Vector3 num3;

	num3.x = num1.x * num2;
	num3.y = num1.y * num2;
	num3.z = num1.z * num2;

	return num3;
}
Vector3 operator*(const float num1, const Vector3& num2) {
	Vector3 num3;

	num3.x = num1 * num2.x;
	num3.y = num1 * num2.y;
	num3.z = num1 * num2.z;

	return num3;
}
Vector3 operator*(const Vector3& num1, const Vector3& num2) {
	Vector3 num3;

	num3.x = num1.x * num2.x;
	num3.y = num1.y * num2.y;
	num3.z = num1.z * num2.z;

	return num3;
}
Vector3 operator/(const Vector3& num1, const float num2) {
	Vector3 num3;

	num3.x = num1.x * num2;
	num3.y = num1.y * num2;
	num3.z = num1.z * num2;

	return num3;
}
Vector3 operator+=(Vector3& num1, const Vector3& num2) {

	num1.x += num2.x;
	num1.y += num2.y;
	num1.z += num2.z;

	return num1;
}
Vector3 operator-=(Vector3& num1, const Vector3& num2) {

	num1.x -= num2.x;
	num1.y -= num2.y;
	num1.z -= num2.z;

	return num1;
}
Vector3 operator*=(Vector3& num1, const float num2) {

	num1.x *= num2;
	num1.y *= num2;
	num1.z *= num2;

	return num1;
}
Vector3 operator*=(Vector3& num1, const Vector3& num2) {

	num1.x *= num2.x;
	num1.y *= num2.y;
	num1.z *= num2.z;

	return num1;
}
Vector3 operator/=(Vector3& num1, const Vector3& num2) {

	num1.x /= num2.x;
	num1.y /= num2.y;
	num1.z /= num2.z;

	return num1;
}
Vector3 operator/=(Vector3& num1, const float num2) {

	num1.x = num1.x * num2;
	num1.y = num1.y * num2;
	num1.z = num1.z * num2;

	return num1;
}
bool operator==(const Vector3& num1, const Vector3& num2) {
	if (num1.x == num2.x
		&& num1.y == num2.y
		&& num1.z == num2.z) {
		return true;
	}

	return false;
}

#pragma endregion

Vector3 Add(const Vector3& v1, const Vector3& v2)
{
	Vector3 result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}


Vector3 Subtract(const Vector3& v1, const Vector3& v2)
{
	Vector3 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}


Vector3 Scalar(float scalar, const Vector3& v)
{
	Vector3 result;
	result.x = scalar * v.x;
	result.y = scalar * v.y;
	result.z = scalar * v.z;
	return result;
}

float Lerp(const float& p0, const float& p1, const float t){
	return (1.0f - t) * p0 + t * p1;
}

Vector3 Lerp(const Vector3& p0, const Vector3& p1, const float t) {

	return Vector3{
		(1.0f - t) * p0.x + t * p1.x,
		(1.0f - t) * p0.y + t * p1.y,
		(1.0f - t) * p0.z + t * p1.z,
	};
}

Vector3 Slerp(const Vector3& p0, const Vector3& p1, const float t)
{
	// 戻り値の宣言
	Vector3 result;

	// 正規化した値を取得
	Vector3 n0 = Normalize(p0);
	Vector3 n1 = Normalize(p1);
	// 内積の取得
	float dot = Dot(n0, n1);
	if (dot < 0) {
		// 逆の回転を利用
		n0 *= (-1.0f);
		dot = -dot;
	}

	// なす角を求める
	float theta = std::acos(dot);
	// sin角を求める
	float sinTheta = 1.0f / std::sinf(theta);

	// thetaとsinを使って補間係数scaler0,scaler1を求める
	// 
	// 0.0により近いか
	if (dot <= -1.0f || 1.0f <= dot || sinTheta == 0.0f) {
		result = n0 * (1.0f - t) + (n1 * t);
	}
	// 近いほうで補完する
	else if (dot < 0.0f) {
		result = (n0 * (std::sin(theta * (1.0f - t)) * sinTheta)) + ((n1*-1) * (std::sin(theta * t) * sinTheta));
	}
	else {
		result = (n0 * (std::sin(theta * (1.0f - t)) * sinTheta)) + (n1 * (std::sin(theta * t) * sinTheta));
	}

	return result;
}

float LerpShortAngle(float s, float e, float t){

	// 角度差分を求める
	float diff = e - s;

	// 回転処理
	const float PI = 3.14f;
	// -2π~2πに
	float rot = std::fmodf(diff, 2.0f * PI);
	if (rot > PI) {
		rot -= 2.0f * PI;
	}
	if (rot < -PI) {
		rot += 2.0f * PI;
	}

	return Lerp(s, s + rot, t);
}

Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, const float t) {
	const float s = 1.0f / 2.0f;	// 数式に出てくる 1/2 のこと
	float t2 = t * t;		// tの2乗
	float t3 = t2 * t;		// tの3乗

	Vector3 e3 = (p0 * (-1)) + (p1 * 3) - (p2 * 3) + p3;
	Vector3 e2 = (p0 * 2) - (p1 * 5) + (p2 * 4) - p3;
	Vector3 e1 = (p0 * (-1)) + p2;
	Vector3 e0 = p1 * 2;

	return s * (e3 * t3 + e2 * t2 + e1 * t + e0);

}

Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t)
{
	assert(points.size() >= 4 && "制御点は4点以上必要です");
	// 区間数は制御点-1
	size_t division = points.size() - 1;
	// 1区間の長さ (全体を1.0としたときの割合)
	float areaWidth = 1.0f / division;

	// 区間内の始点を0.0f、終点を1.0fとしたときの現在位置
	float t_2 = std::fmod(t, areaWidth) * division;
	// 下限(0.0f)と上限(1.0f)の範囲に収める
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	// 区間番号
	size_t index = static_cast<size_t>(t / areaWidth);
	// 区間番号が上限を超えないようにする
	index = std::clamp(index, (size_t)0, division);

	// 4点分のインデックス
	size_t index0 = index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;

	// 最初の区間のp0はp1を重複使用する
	if (index == 0) { index0 = index1; }

	// 最後の区間はp3はp2を重複使用する
	if (index3 >= points.size()) { index3 = index2; }

	const Vector3& p0 = points[index0];
	const Vector3& p1 = points[index1];
	const Vector3& p2 = points[index2];
	const Vector3& p3 = points[index3];

	// 四点を用いてCatmull-Rom補間
	return CatmullRomInterpolation(p0, p1, p2, p3, t_2);

}

float Length(const Vector3& v) {
	float result = 0.0f;
	result = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	return result;
}

/// 正規化
Vector3 Normalize(const Vector3& v)
{
	Vector3 result;
	result.x = v.x / Length(v);
	result.y = v.y / Length(v);
	result.z = v.z / Length(v);

	return result;
}

/// クロス積・外積
Vector3 Cross(const Vector3& v1, const Vector3& v2)
{
	Vector3 result = {
		(v1.y * v2.z) - (v1.z * v2.y),
		(v1.z * v2.x) - (v1.x * v2.z),
		(v1.x * v2.y) - (v1.y * v2.x)
	};

	return result;
};

/// ドット積・内積
float Dot(const Vector3& v1, const Vector3& v2)
{
	return float{ v1.x * v2.x + v1.y * v2.y + v1.z * v2.z };
};

/// 正射影ベクトル
Vector3 Project(const Vector3& v1, const Vector3& v2)
{
	Vector3 result = Scalar(Dot(v1, Normalize(v2)), Normalize(v2));

	return result;
}


Vector3 ClosestPoint(const Vector3& point, const Vector3& origin, const Vector3& diff) {

	float t = Dot(Subtract(point, origin), diff) / pow(Length(diff), 2.0f);
	t = clamp(t, 1.0f, 0.0f);

	Vector3 result = Add(origin, Scalar(t, diff));
	return result;

}

Vector3 Perpendicular(const Vector3& v) {
	if (v.x != 0 || v.y != 0) {
		return { -v.y,v.x,0.0f };
	}

	return{ 0.0f,-v.z,v.y };
}

Vector3 Min(const Vector3& a, const Vector3& b)
{
	Vector3 result{};

	result.x = std::min(a.x, b.x);
	result.y = std::min(a.y, b.y);
	result.z = std::min(a.z, b.z);
	return result;
}

Vector3 Max(const Vector3& a, const Vector3& b)
{
	Vector3 result{};
	result.x = std::max(a.x, b.x);
	result.y = std::max(a.y, b.y);
	result.z = std::max(a.z, b.z);
	return result;
}

Vector3 ExponentialInterpolation(Vector3 a, Vector3 b, float t, float k) {
	Vector3 result = {};
	result.x = a.x + ((b.x - a.x) * k * t);
	result.y = a.y + ((b.y - a.y) * k * t);
	result.z = a.z + ((b.z - a.z) * k * t);
	return result;
}

float ExponentialInterpolation(float a, float b, float t, float k) {
	float result = 0.0f;
	result = a + ((b - a) * k * t);

	return result;
}

float clamp(float num, float max, float min)
{
	if (num > max)
	{
		return max;
	}
	else if (num < min)
	{
		return max;
	}
	else
	{
		return num;
	}
}
