#pragma once

// 前方宣言
struct Vector3;
struct Vector4;
struct Quaternion;

// 
struct Matrix4x4 {
	float m[4][4];
};

/// <summary>
/// 行列の和を返す関数
/// </summary>
/// <param name="matrix1">行列1</param>
/// <param name="matrix2">行列2</param>
/// <returns>"行列1"と"行列2"の積を返す</returns>
Matrix4x4 Add(Matrix4x4 matrix1, Matrix4x4 matrix2);

/// <summary>
/// 行列の差を返す関数
/// </summary>
/// <param name="matrix1">行列1</param>
/// <param name="matrix2">行列2</param>
/// <returns>"行列1"と"行列2"の積を返す</returns>
Matrix4x4 Subtract(Matrix4x4 matrix1, Matrix4x4 matrix2);

/// <summary>
/// 行列の積を返す関数
/// </summary>
/// <param name="matrix1">行列1</param>
/// <param name="matrix2">行列2</param>
/// <returns>"行列1"と"行列2"の積を返す</returns>
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

/// <summary>
/// 行列とベクトルの積を返す関数
/// </summary>
/// <param name="v"></param>
/// <param name="m"></param>
/// <returns></returns>
Vector3 Multiply(const Vector3& v, const Matrix4x4& m);

/// <summary>
/// 逆行列を作成する
/// </summary>
/// <param name="matrix">4x4の正方行列</param>
/// <returns>引数の逆行列を返す</returns>
Matrix4x4 Inverse(Matrix4x4 m);

/// <summary>
/// 転置行列の計算
/// </summary>
/// <param name="matrix">4x4の行列</param>
/// <returns>引数の行列の"列"と"行"を入れ替えた転置行列を返す</returns>
Matrix4x4 Transpose(Matrix4x4 m);

/// <summary>
/// 単位行列の作成
/// </summary>
/// <returns>作成した4x4の単位行列を返す</returns>
Matrix4x4 MakeIdentity();

/// <summary>
// 平行移動行列の作成
/// <summary>
Matrix4x4 MakeTranslateMatrix(const Vector3 translate);

/// <summary>
// 拡大縮小行列の作成
/// <summary>
Matrix4x4 MakeScaleMatrix(const Vector3 scale);

/// <summary>
// 回転行列の作成(X)
/// <summary>
Matrix4x4 MakeRoatateXMatrix(float rad);

/// <summary>
// 回転行列の作成(Y)
/// <summary>
Matrix4x4 MakeRoatateYMatrix(float rad);

/// <summary>
// 回転行列の作成(Z)
/// <summary>
Matrix4x4 MakeRoatateZMatrix(float rad);

/// <summary>
/// 座標変換
/// </summary>
Vector3 Transform(const Vector3& v, const Matrix4x4& m);

/// <summary>
/// 座標変換
/// </summary>
Vector4 Transform(const Vector4& v, const Matrix4x4& m);

/// <summary>
/// ベクトル変換
/// </summary>
/// <param name="v"></param>
/// <param name="m"></param>
/// <returns></returns>
Vector3 TransformNomal(const Vector3& v, const Matrix4x4& m);

/// アフィン行列の作成
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

/// アフィン行列の作成
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& q, const Vector3& translate);

/// <summary>
/// 透視投影行列の作成
/// </summary>
/// <param name="fovY"></param>
/// <param name="aspectRatio"></param>
/// <param name="nearClip"></param>
/// <param name="farClip"></param>
/// <returns></returns>
Matrix4x4 MakePerspectiveMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

/// <summary>
/// 正射影行列の作成
/// </summary>
/// <param name="left"></param>
/// <param name="top"></param>
/// <param name="right"></param>
/// <param name="bottom"></param>
/// <param name="nearClip"></param>
/// <param name="farClip"></param>
/// <returns></returns>
Matrix4x4 MakeOrthographicMatrix(
	float left, float top, float right, float bottom, float nearClip, float farClip);

/// <summary>
/// ビューポート変換行列の作成
/// </summary>
/// <param name="left"></param>
/// <param name="top"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <param name="minDepth"></param>
/// <param name="maxDepth"></param>
/// <returns></returns>
Matrix4x4 MakeViewportMatrix(
	float left, float top, float width, float height, float minDepth, float maxDepth);
