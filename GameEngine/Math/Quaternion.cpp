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
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
	Quaternion result;
	result.x = axis.x * sinf(angle / 2.0f);
	result.y = axis.y * sinf(angle / 2.0f);
	result.z = axis.z * sinf(angle / 2.0f);
	result.w = cosf(angle / 2.0f);
	return result;
}

// ベクトルをQuaternionで回転させた結果のベクトルを求める
Vector3 RotateVector(const Vector3& v, const Quaternion& q) {
	Quaternion r = { v.x,v.y,v.z,0 };
	Quaternion rotate = Multiply(Multiply(q, r), Inverse(q));
	Vector3 result = { rotate.x,rotate.y, rotate.z };
	return result;
}

// Quaternionから回転行列を求める
Matrix4x4 MakeRotateMatrix(const Quaternion& q) {
	float x = q.x, y = q.y, z = q.z, w = q.w;
	float xx = x * x, yy = y * y, zz = z * z, ww = w * w;

	Matrix4x4 result{
		ww + xx - yy - zz,			2.0f * ((x * y) + (w * z)),	2 * ((x * z) - (w * y)),	0.0f,
		2.0f * ((x * y) - (w * z)),	ww - xx + yy - zz,			2 * ((y * z) + (w * x)),	0.0f,
		2.0f * ((x * z) + (w * y)),	2.0f * ((y * z) - (w * x)),	ww - xx - yy + zz,			0.0f,
		0.0f,						0.0f,						0.0f,							1.0f
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

	// 内積を取得 // 単位ベクトルをいれる
	float dot = QDot(Normalize(q0), Normalize(q1));
	// 関数内に正規化したQ0,Q1を生成
	Quaternion q0_ = Normalize(q0);
	Quaternion q1_ = Normalize(q1);
	// 戻り値 宣言
	Quaternion result;
	//確認用変数
	static Quaternion test;

	// 
	if (dot < 0) {
		q0_ = { -q0.x, -q0.y, -q0.z, -q0.w };
		dot = -dot;
	}

	if (dot >= 1.0f - 0.0005f) {
		result = (q0_ * (1.0f - t) + (q1_ * t));
		test = result;
		return  result;
	}
	// なす角を求める
	float theta = std::acos(dot);

	// thetaとsinを使って補間係数scaler0,scaler1を求める
	float scaler0 = std::sin((1.0f - t) * theta) / std::sin(theta);
	float scaler1 = std::sin(t * theta) / std::sin(theta);

	result = (q0_*scaler0) + (q1_*scaler1);

	

	test = result;
	
	return result;

}

float QDot(const Quaternion& q0, const Quaternion& q1) {
	return float{ q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w };
}
