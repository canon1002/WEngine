#include "Vec3.h"
#define _USE_MATH_DEFINES
#include<cmath>
#include<algorithm>
#include <cassert>
#ifdef _DEBUG
#include "Engine/Base/ImGuiManager.h"
#endif // _DEBUG

#pragma region グローバル算術演算子のオーバーロード

Vec3 operator+(const Vec3& num1, const Vec3& num2) {
	Vec3 num3;

	num3.x = num1.x + num2.x;
	num3.y = num1.y + num2.y;
	num3.z = num1.z + num2.z;

	return num3;
}
Vec3 operator-(const Vec3& num1, const Vec3& num2) {
	Vec3 num3;

	num3.x = num1.x - num2.x;
	num3.y = num1.y - num2.y;
	num3.z = num1.z - num2.z;

	return num3;
}
Vec3 operator*(const Vec3& num1, const float num2) {
	Vec3 num3;

	num3.x = num1.x * num2;
	num3.y = num1.y * num2;
	num3.z = num1.z * num2;

	return num3;
}
Vec3 operator*(const float num1, const Vec3& num2) {
	Vec3 num3;

	num3.x = num1 * num2.x;
	num3.y = num1 * num2.y;
	num3.z = num1 * num2.z;

	return num3;
}
Vec3 operator*(const Vec3& num1, const Vec3& num2) {
	Vec3 num3;

	num3.x = num1.x * num2.x;
	num3.y = num1.y * num2.y;
	num3.z = num1.z * num2.z;

	return num3;
}
Vec3 operator/(const Vec3& num1, const float num2) {
	Vec3 num3;

	num3.x = num1.x * num2;
	num3.y = num1.y * num2;
	num3.z = num1.z * num2;

	return num3;
}
Vec3 operator+=(Vec3& num1, const Vec3& num2) {

	num1.x += num2.x;
	num1.y += num2.y;
	num1.z += num2.z;

	return num1;
}
Vec3 operator-=(Vec3& num1, const Vec3& num2) {

	num1.x -= num2.x;
	num1.y -= num2.y;
	num1.z -= num2.z;

	return num1;
}
Vec3 operator*=(Vec3& num1, const float num2) {

	num1.x *= num2;
	num1.y *= num2;
	num1.z *= num2;

	return num1;
}
Vec3 operator*=(Vec3& num1, const Vec3& num2) {

	num1.x *= num2.x;
	num1.y *= num2.y;
	num1.z *= num2.z;

	return num1;
}
Vec3 operator/=(Vec3& num1, const Vec3& num2) {

	num1.x /= num2.x;
	num1.y /= num2.y;
	num1.z /= num2.z;

	return num1;
}
Vec3 operator/=(Vec3& num1, const float num2) {

	num1.x = num1.x * num2;
	num1.y = num1.y * num2;
	num1.z = num1.z * num2;

	return num1;
}
bool operator==(const Vec3& num1, const Vec3& num2) {
	if (num1.x == num2.x
		&& num1.y == num2.y
		&& num1.z == num2.z) {
		return true;
	}

	return false;
}

#pragma endregion

Vec3 Add(const Vec3& v1, const Vec3& v2)
{
	Vec3 result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}


Vec3 Subtract(const Vec3& v1, const Vec3& v2)
{
	Vec3 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}


Vec3 Scalar(float scalar, const Vec3& v)
{
	Vec3 result;
	result.x = scalar * v.x;
	result.y = scalar * v.y;
	result.z = scalar * v.z;
	return result;
}

Vec3 Lerp(const Vec3& p0, const Vec3& p1, const float t) {

	return Vec3{
		t * p0.x + (1.0f - t) * p1.x,
		t * p0.y + (1.0f - t) * p1.y,
		t * p0.z + (1.0f - t) * p1.z
	};
}

Vec3 Slerp(const Vec3& p0, const Vec3& p1, const float t)
{
	//// 正規化した値を取得
	//std::clamp(t, 0.0f, 1.0f);
	//Vec3 np0 = Normalize(p0);
	//Vec3 np1 = Normalize(p1);

	//// なす角をarcsin(アークサイン)を用いて求める
	//float theta = acosf(Dot(np0, np1));
	//float sinTheta = sin(theta);
	//float sinThetaFrom = sin((1 - t) * theta);
	//float sinThetaTo = sin(t * theta);
	//
	//Vec3 result = Lerp(p0, p1, t);
	//Vec3 slerpVector = (sinThetaFrom * np0 + sinThetaTo * np1) / sinTheta;

	//return Vec3(result * slerpVector);

	float newT = std::clamp(t, 0.0f, 1.0f);

	Vec3 normalizeV1 = Normalize(p0);
	Vec3 normalizeV2 = Normalize(p1);


	float dot = Dot(normalizeV1, normalizeV2);

	float theta = std::acosf(dot) * newT;

	Vec3 subtractVec3 = Subtract(p1, p0);
	Vec3 relativeVector = Normalize(
		{ subtractVec3.x * newT,
		subtractVec3.y * newT,
		subtractVec3.z * newT });

	Vec3 result = {
		p0.x * std::cos(theta) + relativeVector.x * std::sin(theta),
		p0.y * std::cos(theta) + relativeVector.y * std::sin(theta),
		p0.z * std::cos(theta) + relativeVector.z * std::sin(theta)
	};

	return result;
}

Vec3 CatmullRomInterpolation(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, const float t) {
	const float s = 1.0f / 2.0f;	// 数式に出てくる 1/2 のこと
	float t2 = t * t;		// tの2乗
	float t3 = t2 * t;		// tの3乗

	Vec3 e3 = (p0 * (-1)) + (p1 * 3) - (p2 * 3) + p3;
	Vec3 e2 = (p0 * 2) - (p1 * 5) + (p2 * 4) - p3;
	Vec3 e1 = (p0 * (-1)) + p2;
	Vec3 e0 = p1 * 2;

	return s * (e3 * t3 + e2 * t2 + e1 * t + e0);

}

Vec3 CatmullRomPosition(const std::vector<Vec3>& points, float t)
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

	const Vec3& p0 = points[index0];
	const Vec3& p1 = points[index1];
	const Vec3& p2 = points[index2];
	const Vec3& p3 = points[index3];

	// 四点を用いてCatmull-Rom補間
	return CatmullRomInterpolation(p0, p1, p2, p3, t_2);

}

float Length(const Vec3& v) {
	float result = 0.0f;
	result = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	return result;
}

/// 正規化
Vec3 Normalize(const Vec3& v)
{
	Vec3 result;
	result.x = v.x / Length(v);
	result.y = v.y / Length(v);
	result.z = v.z / Length(v);

	return result;
}

/// クロス積・外積
Vec3 Cross(const Vec3& v1, const Vec3& v2)
{
	Vec3 result = {
		(v1.y * v2.z) - (v1.z * v2.y),
		(v1.z * v2.x) - (v1.x * v2.z),
		(v1.x * v2.y) - (v1.y * v2.x)
	};

	return result;
};

/// ドット積・内積
float Dot(const Vec3& v1, const Vec3& v2)
{
	return float{ v1.x * v2.x + v1.y * v2.y + v1.z * v2.z };
};

/// 正射影ベクトル
Vec3 Project(const Vec3& v1, const Vec3& v2)
{
	Vec3 result = Scalar(Dot(v1, Normalize(v2)), Normalize(v2));

	return result;
}

/// 最近接点
//Vec3 ClosestPoint(const Vec3& point, const Segment& segment)
//{
//	float t = Dot(Subtract(point, segment.origin), segment.diff) / pow(Length(segment.diff), 2.0f);
//	t = clamp(t, 1.0f, 0.0f);
//
//	Vec3 result = Add(segment.origin, Scalar(t, segment.diff));
//	return result;
//}

Vec3 Perpendicular(const Vec3& v) {
	if (v.x != 0 || v.y != 0) {
		return { -v.y,v.x,0.0f };
	}

	return{ 0.0f,-v.z,v.y };
}
