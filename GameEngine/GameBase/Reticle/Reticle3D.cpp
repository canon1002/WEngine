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
	pInput = InputManager::GetInstance();
	pCamera = MainCamera::GetInstance();


	mObject = std::make_unique<Object3d>();
	mObject->Init("Retcle3D");
	mObject->SetModel("box.gltf");
	//mObject->mWorldTransform->SetParent(pCamera->mWorldTransform->GetWorldMatrix());

	mSprite = std::make_unique<Sprite>();
	mSprite->Initialize();
	mSprite->SetTexture("uvChecker.png");
	mSprite->SetAnchorPoint(Vec2(0.5f, 0.5f));
	mSprite->SetSpriteSize(Vec2(32.0f, 32.0f));

	// 3Dレティクルのワールド座標
	mWorldReticle3D = new WorldTransform();
	mWorldReticle3D->Init();
	//mWorldReticle3D->SetParent(pCamera->mWorldTransform->GetWorldMatrix());
	// 2Dレティクルの座標
	mPostionReticle2D = Vec2(0.0f, 0.0f);

}

void Reticle3D::Update()
{

	// 自機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 50.0f;

	// カメラの回転量から3Dレティクルの位置を計算する
	float pitch = pCamera->GetRotate().x;
	float yaw = pCamera->GetRotate().y;
	float roll = pCamera->GetRotate().z;

	// ピッチとヨーの回転をクォータニオンに変換
	Quaternion qPitch = MakeRotateAxisAngleQuaternion({ 1.0f, 0.0f, 0.0f }, pitch);
	Quaternion qYaw = MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, yaw);
	Quaternion qRoll = MakeRotateAxisAngleQuaternion({ 0.0f, 0.0f, 1.0f }, roll);

	// 初期の方向ベクトル
	Vector3 forward = { 0.0f,0.0f,1.0f };
	// ベクトルをクォータニオンで回転
	Vector3 cameraDirection = RotateVector(RotateVector(RotateVector(forward,qRoll), qPitch), qYaw);

	// レティクルの位置を計算
	mWorldReticle3D->translation = {
		pCamera->GetTranslate().x + cameraDirection.x * kDistancePlayerTo3DReticle,
		pCamera->GetTranslate().y + cameraDirection.y * kDistancePlayerTo3DReticle,
		pCamera->GetTranslate().z + cameraDirection.z * kDistancePlayerTo3DReticle
	};

	// 3Dから2Dへの変換を行う
	const static Matrix4x4 viewPort = MakeViewportMatrix(0, 0,
		pCamera->GetWindowSize().x, pCamera->GetWindowSize().y, 0.0f, 1.0f);


	Matrix4x4 V = pCamera->GetViewMatrix();
	Matrix4x4 P = pCamera->GetProjectionMatrix();
	Matrix4x4 VPV = Multiply(Multiply(V,P), viewPort);

	// スクリーン座標に変換する
	Vector3 screenPos = Transform(mWorldReticle3D->GetWorldPosition(), VPV);
	// スクリーン座標を代入
	mPostionReticle2D = Vec2(screenPos.x, screenPos.y);


	ImGui::Begin("Reticle");
	ImGui::DragFloat2("Reticle2D", &mPostionReticle2D.x, 1.0f, -1280.0f, 1280.0f);
	ImGui::DragFloat3("posNear", &mPosNear.x, 1.0f, -1280.0f, 1280.0f);
	ImGui::DragFloat3("posFar", &mPosFar.x, 1.0f, -1280.0f, 1280.0f);
	ImGui::DragFloat3("Reticle3D S", &mWorldReticle3D->scale.x);
	ImGui::DragFloat3("Reticle3D R", &mWorldReticle3D->rotation.x);
	ImGui::DragFloat3("Reticle3D T", &mWorldReticle3D->translation.x);
	ImGui::End();

	Vector3 pos = mWorldReticle3D->GetWorldPosition();
	mObject->SetTranslate(pos);

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
