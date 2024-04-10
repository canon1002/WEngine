#include "Reticle.h"
#include "Engine/Object/Model/ModelManager.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Base/ImGuiManager.h"

void Reticle::Initialze()
{

	// モデルをセットする
	ModelManager::GetInstance()->LoadModel("box.obj");

	// 2dレティクル
	reticle2d = std::make_unique<Sprite>();
	reticle2d->Initialize();
	reticle2d->SetTexture("Resources/texture/uvChecker.png");
	reticle2d->SetTextureSize({ 64.0f,64.0f });
	reticle2d->SetSpriteSize({ 64.0f,64 });
	reticle2d->SetColor({ 0.3f,0.3f,0.3f,0.4f });
	reticle2d->SetAnchorPoint(Vec2{ 0.5f,0.5f });

	// 3dレティクル
	worldTransformReticle3d.scale = { 1.0f,1.0f,1.0f };
	worldTransformReticle3d.rotate = { 0.0f,0.0f,0.0f };
	worldTransformReticle3d.translate = { 0.0f,0.0f,0.0f };
	worldTransformReticle3d.worldM = MakeAffineMatrix(worldTransformReticle3d.scale,
		worldTransformReticle3d.rotate, worldTransformReticle3d.translate);

	reticle3d = std::make_unique<Object3d>();
	reticle3d->Init();
	reticle3d->SetModel("box.obj");
	reticle3d->SetWorldTransform(worldTransformReticle3d);

	if (camera_ == nullptr) {
		camera_ = MainCamera::GetInstance();
	}


}

void Reticle::Update()
{

#pragma region 3D->2D

#pragma region 自機からのワールド座標から3Dレティクルのワールド座標を計算

	//// 自機から3Dレティクルへの距離
	// const float kDistancePlayerTo3DReticle = 25.0f;
	//// 自機から3Dレティクルへのオフセット(Z+向き)
	// Vec3 offset = {0, 0, 1.0f};
	//// 自機のワールド座標の回転を反映
	// offset = Mat44Funk::Multiply(offset, m_worldTransform.matWorld_);
	//// ベクトルの長さを整える
	// offset = Nomalize(offset);
	// offset.x *= kDistancePlayerTo3DReticle;
	// offset.y *= kDistancePlayerTo3DReticle;
	// offset.z *= kDistancePlayerTo3DReticle;
	//// 3Dレティクルの座標を設定
	// m_worldTransform3DReticle.translation_ = Add(this->GetWorldPos(), offset);

#pragma endregion

#pragma region 3dレティクルから2Dレティクルに変換

	//// ビューポート行列を作成
	// const static Mat44 viewport = Mat44Funk::MakeViewportMatrix(
	//     0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0.0f, 1.0f);

	// Mat44 vp = Mat44Funk::Multiply(
	//     Mat44Funk::Multiply(viewProjection.matView, viewProjection.matProjection),
	//     viewport);

	// Vec3 screenReticlePosition =
	//     Mat44Funk::Transform(m_worldTransform3DReticle.translation_, vp);

	// m_sprite2DReticle->SetPosition({screenReticlePosition.x, screenReticlePosition.y});

	// ImGui::Begin("2DReticle");
	// ImGui::Text("%f\n%f", screenReticlePosition.x, screenReticlePosition.y);
	// ImGui::End();

#pragma endregion

#pragma endregion

#pragma region 2D->3D

	// 左
	if (InputManager::GetInstance()->GetKey()->GetPushKey(DIK_LEFT)||
		Gamepad::getStick((Gamepad::Stick::RIGHT_X)) < -3000) {
		reticle2dPos.x -= 8.0f;
	}

	// 右
	if (InputManager::GetInstance()->GetKey()->GetPushKey(DIK_RIGHT) || 
		Gamepad::getStick((Gamepad::Stick::RIGHT_X)) > 3000) {
		reticle2dPos.x += 8.0f;
	}

	// 下
	if (InputManager::GetInstance()->GetKey()->GetPushKey(DIK_DOWN) || 
		Gamepad::getStick((Gamepad::Stick::RIGHT_Y)) < -3000) {
		reticle2dPos.y += 8.0f;
	}

	// 上
	if (InputManager::GetInstance()->GetKey()->GetPushKey(DIK_UP) || 
		Gamepad::getStick((Gamepad::Stick::RIGHT_Y)) > 3000) {
		reticle2dPos.y -= 8.0f;
	}

	// 移動限界座標
	const float kMoveLimitX = 1240;
	const float kMoveLimitY = 680;
	// 範囲を超えない処理
	reticle2dPos.x = max(reticle2dPos.x, 40);
	reticle2dPos.x = min(reticle2dPos.x, +kMoveLimitX);
	reticle2dPos.y = max(reticle2dPos.y, 40);
	reticle2dPos.y = min(reticle2dPos.y, +kMoveLimitY);
	reticle2d->SetPos(reticle2dPos);
	reticle2d->Update();


	// マウス座標をレティクルの座標に代入
	// m_sprite2DReticle->SetPosition({(float)mousePosition.x, (float)mousePosition.y});

	const static Mat44 viewport = MakeViewportMatrix(
		0, 0, 1280.0f, 720.0f, 0.0f, 1.0f);

	// 先に行列を合成する(逆行列化→合成はNG)
	Mat44 VPV = Multiply(camera_->GetViewProjectionMatrix(), viewport);
	// 逆行列化
	Mat44 inVPV = Inverse(VPV);

	Vec3 posNear = {
		reticle2dPos.x, reticle2dPos.y, 0.0f };
	Vec3 posFar = { reticle2dPos.x, reticle2dPos.y, 1.0f };
	posNear = Transform(posNear, inVPV);
	posFar = Transform(posFar, inVPV);

	Vec3 mouseDirction = Subtract(posFar, posNear);

	// 自機から3Dレティクルへの距離
	const float kDistanceTestObject = 50.0f;

	// ベクトルの長さを整える
	mouseDirction = Normalize(mouseDirction);
	mouseDirction.x *= kDistanceTestObject;
	mouseDirction.y *= kDistanceTestObject;
	mouseDirction.z *= kDistanceTestObject;
	// 3Dレティクルの座標を設定
	worldTransformReticle3d.translate = Add(posNear, mouseDirction);

#ifdef _DEBUG

	ImGui::Begin("Reticle3d");
	ImGui::SliderAngle("RotateX", &worldTransformReticle3d.rotate.x);
	ImGui::SliderAngle("RotateY", &worldTransformReticle3d.rotate.y);
	ImGui::SliderAngle("RotateZ", &worldTransformReticle3d.rotate.z);
	ImGui::DragFloat3("Rotate", &worldTransformReticle3d.rotate.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("Transform", &worldTransformReticle3d.translate.x, 0.1f, -100.0f, 100.0f);
	ImGui::Text("2DReticle:(%f,%f)", reticle2dPos.x, reticle2dPos.y);
	ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", posNear.x, posNear.y, posNear.z);
	ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", posFar.x, posFar.y, posFar.z);
	ImGui::Text(
		"3DReticle:(%+.2f,%+.2f,%+.2f)", worldTransformReticle3d.translate.x,
		worldTransformReticle3d.translate.y, worldTransformReticle3d.translate.z);
	ImGui::End();

#endif // _DEBUG

	worldTransformReticle3d.worldM = MakeAffineMatrix(
		worldTransformReticle3d.scale,
		worldTransformReticle3d.rotate, {
		worldTransformReticle3d.translate.x,
		worldTransformReticle3d.translate.y,
		worldTransformReticle3d.translate.z
		});

	//reticle3d->SetTranslate(worldTransformReticle3d.translate);
	reticle3d->SetWorldTransform(worldTransformReticle3d);
	reticle3d->Update();

#pragma endregion

}

void Reticle::Draw()
{
	reticle2d->Draw();
}

void Reticle::DrawModel()
{
	reticle3d->Draw();
}
