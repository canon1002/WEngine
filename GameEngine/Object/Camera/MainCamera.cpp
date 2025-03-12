#include "MainCamera.h"
#include "GameEngine/Base/WinApp/WinAPI.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/GameMaster/Framerate.h"
#include "App/BlackBoard.h"
#include "GameEngine/Editor/GlobalVariables.h"

MainCamera* MainCamera::instance = nullptr;


MainCamera* MainCamera::GetInstance() {
	if (instance == nullptr) {
		instance = new MainCamera();
	}
	return instance;
}

void MainCamera::Init() {

	mWorldTransform = make_unique<WorldTransform>();
	verticalFOV_ = 0.45f;
	aspectRatio_ = (float(WinApp::GetInstance()->kClientWidth) / float(WinApp::GetInstance()->kClientHeight));
	nearClip_ = 0.1f;
	farClip_ = 1000.0f;
	viewMatrix_ = Inverse(mWorldTransform->GetWorldMatrix());
	projectionMatrix_ = MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_);
	viewprojectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);

	// ターゲット
	mFollowTarget = nullptr; // プレイヤーキャラ
	mSearchTarget = nullptr; // ターゲットとなる敵キャラ

	// フォロー対象が一定時間前にいた座標を保持する変数
	mInterFollowTarget = { 0.0f,0.0f,0.0f };
	// フォロー対象からカメラまでの距離
	mOffset = { 0.0f,0.0f,0.0f };

	// 遷移後回転量
	mEaseBeforeRotation = mWorldTransform->rotation;
	mEasedRotation = mWorldTransform->rotation;
	// 追加平行移動値
	mAddTranslation = { 0.0f,0.0f,0.0f };

	// カメラ回転操作
	mIsControll = true;
	mIsRotationEasing = false;
	mRotaionEasingTime = 0.0f;

	// カメラ回転操作の感度
	mCameraSensitivity = 0.05f;
	// カメラ回転操作の入力経過時間
	mCameraInputCounts = { 0.0f ,0.0f };


}
void MainCamera::Update()
{
	// カメラのオフセットを入力
	MainCamera::GetInstance()->SetOffset(BlackBoard::GetGlobalVariables()->GetVector3Value("Camera", "Offset"));


#ifdef _DEBUG
	if (mWorldTransform != nullptr) {
		ImGui::Begin("MainCamera");
		ImGui::SliderAngle("RotateX", &mWorldTransform->rotation.x);
		ImGui::SliderAngle("RotateY", &mWorldTransform->rotation.y);
		ImGui::SliderAngle("RotateZ", &mWorldTransform->rotation.z);
		ImGui::DragFloat3("Rotate", &mWorldTransform->rotation.x, 0.1f, -1000.0f, 1000.0f);
		ImGui::DragFloat3("Transform", &mWorldTransform->translation.x, 0.1f, -1000.0f, 1000.0f);
		ImGui::DragFloat3("Offset", &mOffset.x, 0.1f, -100.0f, 100.0f);
		ImGui::DragFloat3("AddTransform", &mAddTranslation.x, 0.1f, -1000.0f, 1000.0f);
		ImGui::DragFloat("NearClip", &nearClip_, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("FarClip", &farClip_, 0.1f, 1.0f, 1000.0f);
		ImGui::DragFloat("Sensitivity", &mCameraSensitivity, 0.01f, 0.0f, 10.0f);
		ImGui::End();
	}
#endif // DEBUG

	// フォロー対象がいれば追従を行う
	if (mFollowTarget) {
		
		// 追従座標の補間
		mInterFollowTarget = Lerp(mInterFollowTarget, mFollowTarget->GetWorldPosition(), 0.25f);

		// オフセット分ずらす
		mWorldTransform->translation = mInterFollowTarget + GetOffset();

		// 追加で平行移動値が設定されていれば更にずらす
		mWorldTransform->translation += mAddTranslation;

		Matrix4x4 matW = mWorldTransform->GetWorldMatrix();

		// 回転操作可能であれば回転できるようにする
		if (mIsControll) {

			// スティック入力の量
			const static int stickValue = 8000;

			if (InputManager::GetInstance()->GetStickRatio(Gamepad::Stick::RIGHT_Y, stickValue) != 0.0f ||
				InputManager::GetInstance()->GetStickRatio(Gamepad::Stick::RIGHT_X, stickValue) != 0.0f) {

				// Xの移動量とYの移動量を設定する
				Vector3 direction = {
					InputManager::GetInstance()->GetStickRatio(Gamepad::Stick::RIGHT_Y, stickValue),
					InputManager::GetInstance()->GetStickRatio(Gamepad::Stick::RIGHT_X, stickValue),
					0.0f,
				};
				// 念のために正規化
				//direction = Normalize(direction);
				// スティック上に倒すと下をみるようにする
				direction.x *= (-1.0f);

				// 回転の速度 (感度値を乗算)
				mWorldTransform->rotation.x += direction.x * mCameraSensitivity;
				mWorldTransform->rotation.y += direction.y * mCameraSensitivity;

				// x軸の回転は制限する
				if (mWorldTransform->rotation.x < -0.2f) {
					mWorldTransform->rotation.x = -0.2f;
				}
				if (mWorldTransform->rotation.x > 0.2f) {
					mWorldTransform->rotation.x = 0.2f;
				}

				// y軸の数値修正
				if (mWorldTransform->rotation.y > 3.14f) {
					mWorldTransform->rotation.y = -3.14f;
				}

				if (mWorldTransform->rotation.y < -3.14f) {
					mWorldTransform->rotation.y = 3.14f;
				}


			}
			

		}

	}

	//SetCameraRotarionToSearchTarget();
	UpdateRotationEasing();


	// ビュー行列の更新
	viewMatrix_ = Inverse(mWorldTransform->GetWorldMatrix());
	// プロジェクション行列の更新
	projectionMatrix_ = MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_);
	// 上記２つをビュープロジェクション行列に合成
	viewprojectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);


}

void MainCamera::UpdateRotationEasing()
{
	if (!mIsRotationEasing) {
		return;
	}

	if (mRotaionEasingTime >= 1.0f) {
		mIsRotationEasing = false;
		return;
	}

	if (mRotaionEasingTime < 1.0f) {
		mRotaionEasingTime += BlackBoard::GetBattleFPS();
		if (mRotaionEasingTime > 1.0f) {
			mRotaionEasingTime = 1.0f;
		}
	}

	// Y軸の計算
	if (Length(mSearchTarget->translation - mFollowTarget->translation) != 0.0f) {
		Vector3 direction = Normalize(mSearchTarget->translation - mFollowTarget->translation);

		mEasedRotation.y = atan2f(direction.x, direction.z);
	}

	mWorldTransform->rotation.y = LerpShortAngle(mWorldTransform->rotation.y, mEasedRotation.y, 0.5f);
}

void MainCamera::SetCameraRotarionToSearchTarget()
{
	// カメラ操作不可にする
	mIsControll = true;

	// 追跡対象のポインタ未所持なら 早期リターン
	if (mSearchTarget == nullptr) {
		return;
	}

	// 開始地点の設定
	mEaseBeforeRotation = mWorldTransform->rotation;

	// Y軸の計算
	if (Length(mSearchTarget->translation - mFollowTarget->translation) != 0.0f) {
		Vector3 direction = Normalize(mSearchTarget->translation - mFollowTarget->translation);

		mEasedRotation.y = atan2f(direction.x, direction.z);
	}

	// X軸の計算
	if (Length(mSearchTarget->translation - mWorldTransform->translation) != 0.0f) {
		Vector3 direction = Normalize(mSearchTarget->translation - mWorldTransform->translation);
		mEasedRotation.x = -atan2f(direction.y, direction.z);
	}

	mRotaionEasingTime = 0.0f;
	mIsRotationEasing = true;

}

void MainCamera::SetCameraRotationToDirection(const Vector3 direction)
{
	mEaseBeforeRotation = mWorldTransform->rotation;
	mEasedRotation.y = atan2f(direction.x, direction.z);
	mRotaionEasingTime = 0.0f;
	mIsRotationEasing = true;
}

void MainCamera::SetOffset(const Vector3& offset){
	mOffset = offset;
}

Vector3 MainCamera::GetOffset() const{

	// 追従対象からカメラまでの距離
	Vector3 offset = mOffset;
	// オフセットをカメラの回転に合わせて回転させる
	offset = TransformNomal(offset, mWorldTransform->GetWorldMatrix());

	return offset;
}
