#include "Reticle3D.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Object/Sprite/SpriteAdministrator.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Append/Collider/CollisionManager.h"

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

	// 3Dレティクル
	mObject = std::make_unique<Object3d>();
	mObject->Init("Retcle3D");
	mObject->SetModel("box.gltf");

	// 3Dレティクルのコライダー
	mObject->mCollider = new SphereCollider(mObject->mWorldTransform, 0.5f);
	mObject->mCollider->Init();
	mObject->mCollider->SetCollisionAttribute(kCollisionAttributePlayerBullet);
	mObject->mCollider->SetCollisionMask(kCollisionAttributeEnemy);

	// 自機から3Dレティクルへの距離
	mReticleDistance = 0.5f;

	mSprite = std::make_unique<Sprite>();
	mSprite->Init();
	mSprite->SetTexture("Reticle2D.png");
	mSprite->SetAnchorPoint(Vector2(0.5f, 0.5f));
	mSprite->SetSpriteSize(Vector2(32.0f, 32.0f));

	// 2Dレティクルの座標
	mPostionReticle2D = Vector2(0.0f, 0.0f);

}

void Reticle3D::Update()
{

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

	// レティクルの位置を計算 // Y座標をやや高めの位置に
	mObject->mWorldTransform->translation = {
		pCamera->GetTranslate().x + cameraDirection.x * mReticleDistance,
		pCamera->GetTranslate().y + cameraDirection.y * mReticleDistance,
		pCamera->GetTranslate().z + cameraDirection.z * mReticleDistance
	};

	// 3Dから2Dへの変換を行う
	const static Matrix4x4 viewPort = MakeViewportMatrix(0, 0,
		pCamera->GetWindowSize().x, pCamera->GetWindowSize().y, 0.0f, 1.0f);


	Matrix4x4 V = pCamera->GetViewMatrix();
	Matrix4x4 P = pCamera->GetProjectionMatrix();
	Matrix4x4 VPV = Multiply(Multiply(V,P), viewPort);

	// スクリーン座標に変換する
	Vector3 screenPos = Transform(mObject->mWorldTransform->GetWorldPosition(), VPV);
	// スクリーン座標を代入
	mPostionReticle2D = Vector2(screenPos.x, screenPos.y);

#ifdef _DEBUG

	ImGui::Begin("Reticle");
	ImGui::DragFloat("Distance", &mReticleDistance, 1.0f, -100.0f, 100.0f);
	ImGui::DragFloat2("Reticle2D", &mPostionReticle2D.x, 1.0f, -1280.0f, 1280.0f);
	ImGui::DragFloat3("posNear", &mPosNear.x, 1.0f, -1280.0f, 1280.0f);
	ImGui::DragFloat3("posFar", &mPosFar.x, 1.0f, -1280.0f, 1280.0f);
	ImGui::DragFloat3("Reticle3D S", &mObject->mWorldTransform->scale.x);
	ImGui::DragFloat3("Reticle3D R", &mObject->mWorldTransform->rotation.x);
	ImGui::DragFloat3("Reticle3D T", &mObject->mWorldTransform->translation.x);
	ImGui::End();


#endif // _DEBUG

	mObject->Update();
	mObject->mCollider->Update();
	mObject->DrawGUI();

	mSprite->SetPos(mPostionReticle2D);
	mSprite->Update();
}

void Reticle3D::Draw3DReticle()
{
	//mObject->Draw();
	//mObject->mCollider->Draw();
}

void Reticle3D::Draw2DReticle(){

	SpriteAdministrator::GetInstance()->PreDraw();
	mSprite->Draw();
}