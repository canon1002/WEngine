#include "Quaternion.h"
#include "Math.h"
#include "cmath"

Quaternion Multiply(Quaternion q1, Quaternion q2) {
	return {
		Cross({q1.x,q1.y,q1.z },{q2.x,q2.y,q2.z }).x + q2.w * q1.x + q1.w * q2.x,
		Cross({q1.x,q1.y,q1.z },{q2.x,q2.y,q2.z }).y + q2.w * q1.y + q1.w * q2.y,
		Cross({q1.x,q1.y,q1.z },{q2.x,q2.y,q2.z }).z + q2.w * q1.z + q1.w * q2.z,
		(q1.w * q2.w) - Dot({q1.x,q1.y,q1.z },{q2.x,q2.y,q2.z })
	};
}

Quaternion Inverse(Quaternion q) {
	return{
		Conjugate(q).x / (Norm(q) * Norm(q)),
		Conjugate(q).y / (Norm(q) * Norm(q)),
		Conjugate(q).z / (Norm(q) * Norm(q)),
		Conjugate(q).w / (Norm(q) * Norm(q)),
	};
}

Quaternion IdentityQuaternion() {
	return{
	0.0f,0.0f,0.0f,1.0f
	};
}

Quaternion Normalize(Quaternion q) {
	return{
		q.x / Norm(q),
		q.y / Norm(q),
		q.z / Norm(q),
		q.w / Norm(q),
	};
}

Quaternion Conjugate(Quaternion q) {
	return{
		-q.x,
		-q.y,
		-q.z,
		q.w,
	};
}

float Norm(Quaternion q) {
	return{
		std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w)
	};
}

// 任意回転軸を表すQuaternionの作成
Quaternion MakeRotateAxisAngleQuaternion(const Vec3& axis, float angle) {
	Quaternion result;
	result.x = axis.x * sinf(angle / 2.0f);
	result.y = axis.y * sinf(angle / 2.0f);
	result.z = axis.z * sinf(angle / 2.0f);
	result.w = cosf(angle / 2.0f);
	return result;
}

// ベクトルをQuaternionで回転させた結果のベクトルを求める
Vec3 RotateVector(const Vec3& v, const Quaternion& q) {
	Quaternion r = { v.x,v.y,v.z,0 };
	Quaternion rotate = Multiply(Multiply(q, r), Inverse(q));
	Vec3 result = { rotate.x,rotate.y, rotate.z };
	return result;
}

// Quaternionから回転行列を求める
Mat44 MakeRotateMatrix(const Quaternion& q) {
	float x = q.x, y = q.y, z = q.z, w = q.w;
	float xx = x * x, yy = y * y, zz = z * z, ww = w * w;

	Mat44 result{
		ww + xx - yy - zz,			2 * ((x * y) + (w * z)),	2 * ((x * z) - (w * y)),	0,
		2 * ((x * y) - (w * z)),	ww - xx + yy - zz,			2 * ((y * z) + (w * x)),	0,
		2 * ((x * z) + (w * y)),	2 * ((y * z) - (w * x)),	ww - xx - yy + zz,			0,
		0,							0,							0,							1
	};

	return result;
}


//void QuaternionScreenPrintf(int x, int y, Quaternion& vector, const char* label) {
//
//	//// ラベルの表示
//	//Novice::ScreenPrintf(x + 480, y, "%s", label);
//
//	//// 3次元ベクトルの表示
//	//Novice::ScreenPrintf(x, y, "%6.02f", vector.x);
//	//Novice::ScreenPrintf(x + 80, y, "%6.02f", vector.y);
//	//Novice::ScreenPrintf(x + 160, y, "%6.02f", vector.z);
//	//Novice::ScreenPrintf(x + 240, y, "%6.02f", vector.w);
//
//}

// 球面線形補間
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t) {
	float dot = QDot(q0, q1);
	Quaternion q0_ = q0;
	Quaternion q1_ = q1;
	if (dot < 0) {
		q0_ *= -1;
		dot *= -1;
	}
	
	// なす角を求める
	float theta = std::acos(dot);

	// thetaとsinを使って補間係数scaler0,scaler1を求める
	float scaler0 = std::sin((1.0f - t) * theta) / std::sin(theta);
	float scaler1 = std::sin((t) * theta) / std::sin(theta);
	
	Quaternion result;
	result.x = scaler0 * q0.x + scaler1 * q1.x;
	result.y = scaler0 * q0.y + scaler1 * q1.y;
	result.z = scaler0 * q0.z + scaler1 * q1.z;
	result.w = scaler0 * q0.w + scaler1 * q1.w;
	
	return result;

}

float QDot(const Quaternion& q0, const Quaternion& q1) {
	return float{ q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w };
}