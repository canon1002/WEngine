#include "Reticle3D.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Object/Sprite/SpriteAdministrator.h"

Reticle3D::Reticle3D()
{
}

Reticle3D::~Reticle3D()
{
}

void Reticle3D::Init()
{
}

void Reticle3D::Update()
{
	pInput = InputManager::GetInstance();

	// -- 2Dレティクルの移動処理 -- //
	
	// レティクルを上下左右キーで操作する
	if (pInput->GetPushKey(DIK_LEFT)) {
		mPostionReticle2D.x--;
	}
	if (pInput->GetPushKey(DIK_RIGHT)) {
		mPostionReticle2D.x++;
	}
	if (pInput->GetPushKey(DIK_DOWN)) {
		mPostionReticle2D.y--;
	}
	if (pInput->GetPushKey(DIK_UP)) {
		mPostionReticle2D.y++;
	}

	// -- 2Dレティクルから3Dレティクルへの変換 -- //

	// 2Dから3Dへの変換を行う
	Matrix4x4 viewPort = MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 1.0f);
	Matrix4x4 VPV = Multiply(pCamera->GetViewProjectionMatrix(), viewPort);
	// 逆行列化(行列の合成後に行う)
	Matrix4x4 inVPV = Inverse(VPV);

	// スクリーン座標系
	Vector3 posNear = { mPostionReticle2D.x, mPostionReticle2D.y, 0.0f };
	Vector3 posFar = { mPostionReticle2D.x, mPostionReticle2D.y, 1.0f };

	// ここでワールド座標に変換
	posNear = Transform(posNear, inVPV);
	posFar = Transform(posFar, inVPV);

	// 上記で宣言した二点を用いて方向を得る
	Vector3 mouseDirction = Subtract(posFar, posNear);
	// 正規化してベクトルの長さを整える
	mouseDirction = Normalize(mouseDirction);

	// カメラからどれくらいの長さで進むか設定
	const float kDistance = 0.01f;
	// 移動量の宣言
	Vector3 cameraMoveVelotity = { 0.0f,0.0f,1.0f };
	// 移動量は方向*移動距離
	cameraMoveVelotity.x = mouseDirction.x * kDistance;
	cameraMoveVelotity.y = mouseDirction.y * kDistance;
	cameraMoveVelotity.z = mouseDirction.z * kDistance;
	// 終点に結果を加算
	mWorldReticle3D->translation = Add(posNear, cameraMoveVelotity);


}

void Reticle3D::Draw3DReticle()
{
}

void Reticle3D::Draw2DReticle(){


}
