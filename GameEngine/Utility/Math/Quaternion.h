#pragma once

struct Vector3;
struct Matrix4x4;

struct Quaternion
{
	float x, y, z, w;

	Quaternion operator+(const Quaternion q) const{
		Quaternion r;
		r.x = this->x + q.x;
		r.y = this->y + q.y;
		r.z = this->z + q.z;
		r.w = this->w + q.w;
		return r;
	}

	Quaternion operator*(Quaternion other) {
		Quaternion r{};
		r.x = this->x * other.x;
		r.y = this->y * other.y;
		r.z = this->z * other.z;
		r.w = this->w * other.w;
		return r;
	}

	Quaternion operator*(const float s) {
		Quaternion r;
		r.x = this->x * s;
		r.y = this->y * s;
		r.z = this->z * s;
		r.w = this->w * s;
		return r;
	}
};

Quaternion Multiply(Quaternion q1, Quaternion q2);
Quaternion Inverse(Quaternion q);
Quaternion IdentityQuaternion();

Quaternion Normalize(Quaternion q);
Quaternion Conjugate(Quaternion q);
float Norm(Quaternion q);
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);
float QDot(const Quaternion& q0, const Quaternion& q1);
float Length(Quaternion q);

// 任意回転軸を表すQuaternionの作成
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);
// ベクトルをQuaternionで回転させた結果のベクトルを求める
Vector3 RotateVector(const Vector3& v, const Quaternion& q);
// Quaternionから回転行列を求める
Matrix4x4 MakeRotateMatrix(const Quaternion& q);


//void QuaternionScreenPrintf(int x, int y, Quaternion& vector, const char* label);