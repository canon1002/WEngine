#include "MainCamera.h"
#include "Base/WinApp/WinApp.h"
#include "Base/DirectX/DirectXCommon.h"
#include "Editor/ImGui/ImGuiManager.h"
#include "Input/InputManager.h"

MainCamera* MainCamera::instance = nullptr;


MainCamera* MainCamera::GetInstance(){
	if (instance == nullptr) {
		instance = new MainCamera();
	}
	return instance;
}

void MainCamera::Init(std::string objectName){


	WinApp* winApp = WinApp::GetInstance();

	// オブジェクト名を設定
	mObjectName = objectName;

	mWorldTransform = WorldTransform();
	verticalFOV_ = 0.45f;
	aspectRatio_ = (float(winApp->kClientWidth) / float(winApp->kClientHeight));
	nearClip_ = 0.1f;
	farClip_ = 1000.0f;
	mViewMatrix = Inverse(mWorldTransform.GetWorldMatrix());
	mProjectionMatrix_ = MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_);
	mViewprojectionMatrix_ = Multiply(mViewMatrix, mProjectionMatrix_);

	// ターゲット
	mTarget = nullptr;
	// 追加平行移動値
	mAddTranslation = { 0.0f,0.0f,0.0f };
}

void MainCamera::Update()
{
	// 入力を取得する
	InputManager* input = InputManager::GetInstance();


	// フォロー対象がいれば追従を行う
	if (mTarget) {
		// 追従対象からカメラまでの距離
		Vector3 offset = { 0.0f,1.2f,0.5f };
		// オフセットをカメラの回転に合わせて回転させる
		offset = TransformNomal(offset, mWorldTransform.GetWorldMatrix());
		// オフセット分ずらす
		mWorldTransform.translation = mTarget->translation + offset;
		
		// 追加で平行移動値が設定されていれば更にずらす
		mWorldTransform.translation += mAddTranslation;

		Matrix4x4 matW = mWorldTransform.GetWorldMatrix();

		// スティック入力の量
		const static int stickValue = 6000;

		// 入力量に応じた回転を行う
		if (input->GetStick(Gamepad::Stick::RIGHT_X) < -stickValue || // 左 
			input->GetStick(Gamepad::Stick::RIGHT_X) > stickValue || // 右
			input->GetStick(Gamepad::Stick::RIGHT_Y) < -stickValue || // 上
			input->GetStick(Gamepad::Stick::RIGHT_Y) > stickValue	  // 下
			) {

			// Xの移動量とYの移動量を設定する
			Vector3 direction = {
				(float)input->GetStick(Gamepad::Stick::RIGHT_Y),
				(float)input->GetStick(Gamepad::Stick::RIGHT_X),
				0.0f,
			};
			// 念のために正規化
			direction = Normalize(direction);
			// スティック上に倒すと下をみるようにする
			direction.x *= (-1.0f);

			// 回転の速度 // メンバ変数にしても良さそう
			float rotateSpeed = 0.01f;
			mWorldTransform.rotation += direction * rotateSpeed;

			// x軸の回転は制限する
			if (mWorldTransform.rotation.x < -0.2f) {
				mWorldTransform.rotation.x = -0.2f;
			}
			if (mWorldTransform.rotation.x > 0.2f) {
				mWorldTransform.rotation.x = 0.2f;
			}


		}

	}

	// ビュー行列の更新
	mViewMatrix = Inverse(mWorldTransform.GetWorldMatrix());
	// プロジェクション行列の更新
	mProjectionMatrix_ = MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_);
	// 上記２つをビュープロジェクション行列に合成
	mViewprojectionMatrix_ = Multiply(mViewMatrix, mProjectionMatrix_);


}

void MainCamera::Debug()
{
#ifdef _DEBUG

	ImGui::Begin("MainCamera");
	ImGui::SliderAngle("RotateX", &mWorldTransform.rotation.x);
	ImGui::SliderAngle("RotateY", &mWorldTransform.rotation.y);
	ImGui::SliderAngle("RotateZ", &mWorldTransform.rotation.z);
	ImGui::DragFloat3("Rotate", &mWorldTransform.rotation.x, 0.1f, -1000.0f, 1000.0f);
	ImGui::DragFloat3("Transform", &mWorldTransform.translation.x, 0.1f, -1000.0f, 1000.0f);
	ImGui::DragFloat("NearClip", &nearClip_, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("FarClip", &farClip_, 0.1f, 1.0f, 1000.0f);
	ImGui::End();


#endif // DEBUG

}
