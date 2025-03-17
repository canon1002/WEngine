#include "Vec2.h"

bool InRange(float value, Vector2 range)
{
	if (range.x <= value && value <= range.y) {
		return true;
	}
	return false;
}

Vector2 Scalar(float scalar, const Vector2& v)
{
	Vector2 result{};
	result.x = scalar * v.x;
	result.y = scalar * v.y;
	return result;
}