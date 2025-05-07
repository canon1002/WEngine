#pragma once
struct Vector2{
	float x, y;
};

// 2点間の中に値があるか計算する
bool InRange(float value, Vector2 range);

// スカラー倍
Vector2 Scalar(float scalar, const Vector2& v);