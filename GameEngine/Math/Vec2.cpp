#include "Vec2.h"

bool InRange(float value, Vector2 range)
{
	if (range.x <= value && value <= range.y) {
		return true;
	}
	return false;
}
