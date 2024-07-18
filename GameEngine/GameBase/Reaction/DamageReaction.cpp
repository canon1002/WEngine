#include "DamageReaction.h"
#include "GameEngine/Object/Camera/MainCamera.h"

DamageReaction* DamageReaction::instance = nullptr;

DamageReaction* DamageReaction::GetInstance(){

	if (instance == nullptr) {
		instance = new DamageReaction();
	}

	return instance;
}

void DamageReaction::Reaction(const Vector3 pos, const MainCamera* camera){
	
	// ワールド座標をスクリーン座標に変換する
	Vec2 screenPos = GetScreenPos(pos, camera);

}

Vec2 DamageReaction::GetScreenPos(const Vector3 pos, const MainCamera* camera)
{

	// 3Dから2Dへの変換を行う
	const static Matrix4x4 viewPort = MakeViewportMatrix(0, 0,
		camera->GetWindowSize().x, camera->GetWindowSize().y, 0.0f, 1.0f);

	Matrix4x4 V = camera->GetViewMatrix();
	Matrix4x4 P = camera->GetProjectionMatrix();
	Matrix4x4 VPV = Multiply(Multiply(V, P), viewPort);

	// スクリーン座標に変換する
	Vector3 screenPos = Transform(pos, VPV);

	return Vec2(screenPos.x,screenPos.y);
}
