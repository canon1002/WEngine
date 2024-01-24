#pragma once
#include "../../math/Math.h"

struct Transformation {
	Vec3 scale;
	Vec3 rotate;
	Vec3 translate;

	Transformation operator+(Transformation obj) {
		Transformation tr;
		// 拡大縮小
		tr.scale.x += obj.scale.x;
		tr.scale.y += obj.scale.y;
		tr.scale.z += obj.scale.z;
		// 回転
		tr.rotate.x += obj.rotate.x;
		tr.rotate.y += obj.rotate.y;
		tr.rotate.z += obj.rotate.z;
		// 平行移動
		tr.translate.x += obj.translate.x;
		tr.translate.y += obj.translate.y;
		tr.translate.z += obj.translate.z;
		return tr;
	}
	Transformation operator-(Transformation obj) {
		Transformation tr;
		// 拡大縮小
		tr.scale.x -= obj.scale.x;
		tr.scale.y -= obj.scale.y;
		tr.scale.z -= obj.scale.z;
		// 回転
		tr.rotate.x -= obj.rotate.x;
		tr.rotate.y -= obj.rotate.y;
		tr.rotate.z -= obj.rotate.z;
		// 平行移動
		tr.translate.x -= obj.translate.x;
		tr.translate.y -= obj.translate.y;
		tr.translate.z -= obj.translate.z;
		return tr;
	}
};

struct WorldTransform
{ 

	WorldTransform(){
		// 拡大
		scale = { 1.0f,1.0f,1.0f };
		// 縮小
		rotate = { 0.0f,0.0f,0.0f };
		// 平行移動
		translate = { 0.0f,0.0f,0.0f };
		// 行列
		worldM = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
	}

	// 拡大
	Vec3 scale;
	// 縮小
	Vec3 rotate;
	// 平行移動
	Vec3 translate;
	// 行列
	Mat44 worldM;


	Vec3 GetScale() { return{ worldM.m[0][0],worldM.m[0][1] ,worldM.m[0][2] }; }
	Vec3 GetRotate() { return{ worldM.m[1][0],worldM.m[1][1] ,worldM.m[1][2] }; }
	Vec3 GetTranslate() { return{ worldM.m[2][0],worldM.m[2][1] ,worldM.m[2][2] }; }
};

