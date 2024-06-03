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

float Length(Quaternion q) {
	return sqrtf(powf(q.w, 2) + powf(q.x, 2) + powf(q.y, 2) + powf(q.z, 2));
}

Quaternion Inverse(Quaternion q) {
	Quaternion result{};
	// 正規化するベクトルの長さを求める
	float length = Length(q);
	length = std::powf(length, 2);
	// 共役クォータニオンを求める
	Quaternion conjugate = Conjugate(q);

	// 計算処理
	if (length != 0.0f) {
		result.x = conjugate.x / length;
		result.y = conjugate.y / length;
		result.z = conjugate.z / length;
		result.w = conjugate.w / length;
	}

	// 結果を返す
	return result;
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
	return std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

// 任意回転軸を表すQuaternionの作成
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
	Quaternion result{};
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
	// 返却用の変数 // 単位行列を生成
	Matrix4x4 result = MakeIdentity();

	// 計算処理を行う
	// 下記に記載のない要素は単位行列の数値のままとなっている
	result.m[0][0] = powf(q.w, 2) + powf(q.x, 2) - powf(q.y, 2) - powf(q.z, 2);
	result.m[0][1] = 2.0f * ((q.x * q.y) + (q.w * q.z));
	result.m[0][2] = 2.0f * ((q.x * q.z) - (q.w * q.y));
	
	
	result.m[1][0] = 2.0f * ((q.x * q.y) - (q.w * q.z));
	result.m[1][1] = powf(q.w, 2) - powf(q.x, 2) + powf(q.y, 2) - powf(q.z, 2);
	result.m[1][2] = 2.0f * (q.y * q.z + q.w * q.x);
	
	
	result.m[2][0] = 2.0f * (q.x * q.z + q.w * q.y);
	result.m[2][1] = 2.0f * (q.y * q.z - q.w * q.x);
	result.m[2][2] = powf(q.w, 2) - powf(q.x, 2) - powf(q.y, 2) + powf(q.z, 2);


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

	// 関数内に正規化したQ0,Q1を生成
	Quaternion q0_ = (q0);
	Quaternion q1_ = (q1);
	// 内積を取得 // 単位ベクトルをいれる
	float dot = QDot(q0,q1);
	// 戻り値 宣言
	Quaternion result;

	if (dot < 0) {
		// 逆の回転を利用
		q0_ = Inverse(q0_);
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
		result = q0_ * (1.0f - t) + (q1_ * t);
	}
	// 近いほうで補完する
	else if (dot < 0.0f) {
		result = (q0_ * (std::sin(theta * (1.0f - t)) * sinTheta)) + (Inverse(q1) * (std::sin(theta * t) * sinTheta));
	}
	else {
		result = (q0_ * (std::sin(theta * (1.0f - t)) * sinTheta)) + (q1_ * (std::sin(theta * t) * sinTheta));
	}
	
	return result;

}

float QDot(const Quaternion& q0, const Quaternion& q1) {
	return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
}
