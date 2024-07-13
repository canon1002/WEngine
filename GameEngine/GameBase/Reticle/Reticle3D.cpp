#include "Reticle3D.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Object/Sprite/SpriteAdministrator.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

Reticle3D::Reticle3D()
{
}

Reticle3D::~Reticle3D()
{
}

void Reticle3D::Init()
{
	mObject = std::make_unique<Object3d>();
	mObject->Init("Retcle3D");
	mObject->SetModel("box.gltf");

	mSprite = std::make_unique<Sprite>();
	mSprite->Initialize();
	mSprite->SetTexture("uvChecker.png");
	mSprite->SetAnchorPoint(Vec2(0.5f, 0.5f));
	mSprite->SetSpriteSize(Vec2(32.0f, 32.0f));

	// 3Dレティクルのワールド座標
	mWorldReticle3D = new WorldTransform();
	mWorldReticle3D->Init();
	// 2Dレティクルの座標
	mPostionReticle2D = Vec2(640.0f, 360.0f);

}

void Reticle3D::Update()
{
	pInput = InputManager::GetInstance();
	pCamera = MainCamera::GetInstance();

	
	// -- 2Dレティクルから3Dレティクルへの変換 -- //

	// 2Dから3Dへの変換を行う
	Matrix4x4 viewPort = MakeViewportMatrix(0, 0, 
		pCamera->GetWindowSize().x, pCamera->GetWindowSize().y, 0.0f, 1.0f);
	Matrix4x4 VPV = Multiply(pCamera->GetProjectionMatrix(), viewPort);
	// 逆行列化(行列の合成後に行う)
	Matrix4x4 inVPV = Inverse(VPV);
	// スクリーン座標系
	mPosNear = { mPostionReticle2D.x, mPostionReticle2D.y, 0.0f };
	mPosFar = { mPostionReticle2D.x, mPostionReticle2D.y, 1.0f };

	// ここでワールド座標に変換
	mPosNear = Transform(mPosNear, inVPV);
	mPosFar = Transform(mPosFar, inVPV);

	// 上記で宣言した二点を用いて方向を得る
	Vector3 reticleDirction = Subtract(mPosFar, mPosNear);
	// 正規化してベクトルの長さを整える
	reticleDirction = Normalize(reticleDirction);

	// カメラからどれくらいの長さで進むか設定
	const float kDistance = 30.0f;
	// 移動量は方向*移動距離
	reticleDirction.x *= kDistance;
	reticleDirction.y *= kDistance;
	reticleDirction.z *= kDistance;
	// 終点に結果を加算
	mWorldReticle3D->translation = Add(mPosNear, reticleDirction);

	ImGui::Begin("Reticle");
	ImGui::DragFloat2("Reticle2D", &mPostionReticle2D.x, 1.0f, -1280.0f, 1280.0f);
	ImGui::DragFloat3("posNear", &mPosNear.x, 1.0f, -1280.0f, 1280.0f);
	ImGui::DragFloat3("posFar", &mPosFar.x, 1.0f, -1280.0f, 1280.0f);
	ImGui::DragFloat3("Reticle3D S", &mWorldReticle3D->scale.x);
	ImGui::DragFloat3("Reticle3D R", &mWorldReticle3D->rotation.x);
	ImGui::DragFloat3("Reticle3D T", &mWorldReticle3D->translation.x);
	ImGui::End();

	mObject->mWorldTransform->translation = mWorldReticle3D->translation;
	mObject->mWorldTransform->SetParent(pCamera->mWorldTransform->GetWorldMatrix());

	mObject->Update();
	mObject->DrawGUI();

	mSprite->SetPos(mPostionReticle2D);
	mSprite->Update();
}

void Reticle3D::Draw3DReticle()
{
	mObject->Draw();
}

void Reticle3D::Draw2DReticle(){

	SpriteAdministrator::GetInstance()->PreDraw();
	mSprite->Draw();
}
