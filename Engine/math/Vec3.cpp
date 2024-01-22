#include "Vec3.h"
#define _USE_MATH_DEFINES
#include<cmath>
#include<algorithm>

Vec3 operator+(Vec3 obj) {
	Vec3 v;
	// 拡大縮小
	v.x += obj.x;
	v.y += obj.y;
	v.z += obj.z;
	return v;
}
Vec3& operator+=(Vec3& a, const Vec3& b) {
	// 拡大縮小
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

Vec3 operator-(Vec3 obj) {
	Vec3 v;
	// 拡大縮小
	v.x -= obj.x;
	v.y -= obj.y;
	v.z -= obj.z;
	return v;
}
Vec3 operator*(Vec3 obj, float f) {
	Vec3 v;
	// スカラー倍を行う
	v.x = obj.x * f;
	v.y = obj.y * f;
	v.z = obj.z * f;
	return v;
}

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

Vec3 Leap(const Vec3& p0, const Vec3& p1, const float t) {

	return Vec3{
		t * p0.x + (1.0f - t) * p1.x,
		t * p0.y + (1.0f - t) * p1.y,
		t * p0.z + (1.0f - t) * p1.z
	};
}

/// ベクトルの長さを求める
float Length(const Vec3& v) {
	float result;
	result = sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	return result;
}

/// 正規化
Vec3 Nomalize(const Vec3& v)
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
	Vec3 result = Scalar(Dot(v1, Nomalize(v2)), Nomalize(v2));

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
