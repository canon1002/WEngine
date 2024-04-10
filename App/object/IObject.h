#pragma once
#include "Engine/Math/Math.h"

class IObject
{
public:

	// 純粋仮想関数
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual ~IObject();

};

