#pragma once
#include "../../math/Math.h"

class WorldTransform
{
public: 

	WorldTransform(){
		// 拡大
		scale = { 1.0f,1.0f,1.0f };
		// 縮小
		rotate = { 0.0f,0.0f,0.0f };
		// 平行移動
		translate = { 0.0f,0.0f,0.0f };
		// 行列
		worldM = W::Math::MakeAffineMatrix();
	}

	// 拡大
	Vector3 scale;
	// 縮小
	Vector3 rotate;
	// 平行移動
	Vector3 translate;
	// 行列
	Matrix4x4 worldM;


	Vector3 GetScale() { return{ worldM.m[0][0],worldM.m[0][1] ,worldM.m[0][2] }; }
	Vector3 GetRotate() { return{ worldM.m[1][0],worldM.m[1][1] ,worldM.m[1][2] }; }
	Vector3 GetTranslate() { return{ worldM.m[2][0],worldM.m[2][1] ,worldM.m[2][2] }; }
};

