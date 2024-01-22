#pragma once

// 前方宣言
struct Vec3;
struct Vec4;

// 
struct Mat44{
	float m[4][4];
};

/// <summary>
 /// 行列の和を返す関数
 /// </summary>
 /// <param name="matrix1">行列1</param>
 /// <param name="matrix2">行列2</param>
 /// <returns>"行列1"と"行列2"の積を返す</returns>
Mat44 Add(Mat44 matrix1, Mat44 matrix2);

/// <summary>
/// 行列の差を返す関数
/// </summary>
/// <param name="matrix1">行列1</param>
/// <param name="matrix2">行列2</param>
/// <returns>"行列1"と"行列2"の積を返す</returns>
Mat44 Subtract(Mat44 matrix1, Mat44 matrix2);

/// <summary>
/// 行列の積を返す関数
/// </summary>
/// <param name="matrix1">行列1</param>
/// <param name="matrix2">行列2</param>
/// <returns>"行列1"と"行列2"の積を返す</returns>
Mat44 Multiply(Mat44 matrix1, Mat44 matrix2);

/// <summary>
/// 行列とベクトルの積を返す関数
/// </summary>
/// <param name="v"></param>
/// <param name="m"></param>
/// <returns></returns>
Vec3 Multiply(const Vec3& v, const Mat44& m);

/// <summary>
/// 逆行列を作成する
/// </summary>
/// <param name="matrix">4x4の正方行列</param>
/// <returns>引数の逆行列を返す</returns>
Mat44 Inverse(Mat44 m);

/// <summary>
/// 転置行列の計算
/// </summary>
/// <param name="matrix">4x4の行列</param>
/// <returns>引数の行列の"列"と"行"を入れ替えた転置行列を返す</returns>
Mat44 Transpose(Mat44 m);

/// <summary>
/// 単位行列の作成
/// </summary>
/// <returns>作成した4x4の単位行列を返す</returns>
Mat44 MakeIdentity();

/// <summary>
// 平行移動行列の作成
/// <summary>
Mat44 MakeTranslateMatrix(const Vec3 translate);

/// <summary>
// 拡大縮小行列の作成
/// <summary>
Mat44 MakeScaleMatrix(const Vec3 scale);

/// <summary>
// 回転行列の作成(X)
/// <summary>
Mat44 MakeRoatateXMatrix(float rad);

/// <summary>
// 回転行列の作成(Y)
/// <summary>
Mat44 MakeRoatateYMatrix(float rad);

/// <summary>
// 回転行列の作成(Z)
/// <summary>
Mat44 MakeRoatateZMatrix(float rad);

/// <summary>
/// 座標変換
/// </summary>
Vec3 Transform(const Vec3& v, const Mat44& m);

/// <summary>
/// 座標変換
/// </summary>
Vec4 Transform(const Vec4& v, const Mat44& m);

/// <summary>
/// ベクトル変換
/// </summary>
/// <param name="v"></param>
/// <param name="m"></param>
/// <returns></returns>
Vec3 TransformNomal(const Vec3& v, const Mat44& m);

/// <summary>
/// アフィン行列の作成
/// <summary>
Mat44 MakeAffineMatrix(const Vec3& scale, const Vec3& rotate, const Vec3& translate);

/// <summary>
/// 透視投影行列の作成
/// <summary>
Mat44 MakePerspectiveMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

/// <summary>
/// 正射影行列の作成
/// <summary>
Mat44 MakeOrthographicMatrix(
    float left, float top, float right, float bottom, float nearClip, float farClip);

/// <summary>
/// ビューポート変換行列の作成
/// <summary>
Mat44 MakeViewportMatrix(
    float left, float top, float width, float height, float minDepth, float maxDepth);
