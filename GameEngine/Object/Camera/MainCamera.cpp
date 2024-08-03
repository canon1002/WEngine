#include "MainCamera.h"
#include "GameEngine/Base/WinApp/WinAPI.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

MainCamera* MainCamera::instance = nullptr;


MainCamera* MainCamera::GetInstance(){
	if (instance == nullptr) {
		instance = new MainCamera();
	}
	return instance;
}

void MainCamera::Initialize(){
	winApp_ = WinAPI::GetInstance();
	// 入力を取得する
	mInput = InputManager::GetInstance();
	mWorldTransform = new WorldTransform();
	verticalFOV_ = 0.45f;
	aspectRatio_ = (float(winApp_->kClientWidth) / float(winApp_->kClientHeight));
	nearClip_ = 0.01f;
	farClip_ = 1000.0f;
	viewMatrix_ = Inverse(mWorldTransform->GetWorldMatrix());
	projectionMatrix_ = MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_);
	viewprojectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);

	// ターゲット
	mTarget = nullptr;
	// 追加平行移動値
	mAddTranslation = { 0.0f,0.0f,0.0f };
}
void MainCamera::Update()
{
#ifdef _DEBUG
	if (mWorldTransform != nullptr) {
		ImGui::Begin("MainCamera");
		ImGui::SliderAngle("RotateX", &mWorldTransform->rotation.x);
		ImGui::SliderAngle("RotateY", &mWorldTransform->rotation.y);
		ImGui::SliderAngle("RotateZ", &mWorldTransform->rotation.z);
		ImGui::DragFloat3("Rotate", &mWorldTransform->rotation.x, 0.1f, -1000.0f, 1000.0f);
		ImGui::DragFloat3("Transform", &mWorldTransform->translation.x, 0.1f, -1000.0f, 1000.0f);
		ImGui::DragFloat("NearClip", &nearClip_, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("FarClip", &farClip_, 0.1f, 1.0f, 1000.0f);
		ImGui::End();
	}
#endif // DEBUG

	// フォロー対象がいれば追従を行う
	if (mTarget) {
		// 追従対象からカメラまでの距離
		Vector3 offset = { 0.0f,1.2f,0.5f };
		// オフセットをカメラの回転に合わせて回転させる
		offset = TransformNomal(offset, mWorldTransform->GetWorldMatrix());
		// オフセット分ずらす
		mWorldTransform->translation = mTarget->translation + offset;
		
		// 追加で平行移動値が設定されていれば更にずらす
		mWorldTransform->translation += mAddTranslation;

		Matrix4x4 matW = mWorldTransform->GetWorldMatrix();

		// スティック入力の量
		const static int stickValue = 6000;

		// 入力量に応じた回転を行う
		if (mInput->GetStick(Gamepad::Stick::RIGHT_X) < -stickValue || // 左 
			mInput->GetStick(Gamepad::Stick::RIGHT_X) > stickValue || // 右
			mInput->GetStick(Gamepad::Stick::RIGHT_Y) < -stickValue || // 上
			mInput->GetStick(Gamepad::Stick::RIGHT_Y) > stickValue	  // 下
			) {

			// Xの移動量とYの移動量を設定する
			Vector3 direction = {
				(float)mInput->GetStick(Gamepad::Stick::RIGHT_Y),
				(float)mInput->GetStick(Gamepad::Stick::RIGHT_X),
				0.0f,
			};
			// 念のために正規化
			direction = Normalize(direction);
			// スティック上に倒すと下をみるようにする
			direction.x *= (-1.0f);

			// 回転の速度 // メンバ変数にしても良さそう
			float rotateSpeed = 0.01f;
			mWorldTransform->rotation += direction * rotateSpeed;
		}

	}

	// ビュー行列の更新
	viewMatrix_ = Inverse(mWorldTransform->GetWorldMatrix());
	// プロジェクション行列の更新
	projectionMatrix_ = MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_);
	// 上記２つをビュープロジェクション行列に合成
	viewprojectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);


}
