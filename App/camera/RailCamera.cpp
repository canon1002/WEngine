#include "RailCamera.h"
#include"Engine/Base/ImGuiManager.h"

void RailCamera::Initialize() {

	this->SetRotate(Vec3{ 0.0f,0.0f,0.0f });
	this->SetTranslate(Vec3{ 0.0f,0.0f,0.0f });

	// スプライン曲線の制御点
	// 設定する
	sprineControlPoints_ = {
		{0,	 0,	 0},
		{10, 0,	 10},
		{10, 0,  20},
		{20, 0,  30},
		{20, 0,	 40},
		{10, 0,	 50},
		{0,	 0,	 0},
	};

	t = 0.0f;
	forwardT = 0.1f;
}

void RailCamera::Initialize(Vec3 tr, Vec3 r) {

	this->SetRotate(r);
	this->SetTranslate(tr);

}


void RailCamera::Update() {

#ifdef _DEBUG

	ImGui::Begin("Camera");
	ImGui::SliderAngle("RotateX", &transform_.rotate.x);
	ImGui::SliderAngle("RotateY", &transform_.rotate.y);
	ImGui::SliderAngle("RotateZ", &transform_.rotate.z);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("Transform", &transform_.translate.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat("t", &t, 0.005f, 0.0f, 1.0f);
	ImGui::End();

#endif // _DEBUG


#pragma region カメラの回転/向きの変更

	//// 始点と終点の二点を用いて方向を得る
	//Vec3 direction = Subtract(railWorld_.translate, transform_.translate);
	//// 正規化してベクトルの長さを整える
	//direction = Normalize(direction);
	//const float PI = 3.14f;

	//// プレイヤーの回転
	//// 時計周りでプラス
	////const float ROTATE_AMOUNT = 0.01f;
	//Vec3 rotateMove = {};
	//rotateMove.y = std::atan2f(direction.x, direction.z);
	//// 底辺の長さ
	//float lengthBase = Length(Vec3(direction.x, 0.0f, direction.z));
	//rotateMove.x = std::atan2f(lengthBase, direction.y);
	////rotateMove.y = std::fmodf(rotateMove.y, 2.0f * PI);
	//if (rotateMove.x > PI) {rotateMove.y -= 2.0f * PI;}
	//if (rotateMove.x < -PI) {rotateMove.y += 2.0f * PI;}
	//if (rotateMove.y > PI) { rotateMove.y -= 2.0f * PI; }
	//if (rotateMove.y < -PI) { rotateMove.y += 2.0f * PI; }
	////transform_.rotate = rotateMove;

#pragma endregion


	// 補間数値の加算
	if (t < 1.0f) { t += 0.001f; }
	if (t > 1.0f) { t = 0.0f; }
	if (forwardT < 1.0f) { forwardT += 0.001f;}
	if (forwardT > 1.0f) { forwardT = 0.0f;}

	// 制御点とtから座標を取得する
	transform_.translate = CatmullRomPosition(sprineControlPoints_, t);
	railWorld_.translate = CatmullRomPosition(sprineControlPoints_, forwardT);


	// ワールド行列の計算
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	railWorld_.worldM = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	// ビュー行列の更新
	viewMatrix_ = Inverse(railWorld_.worldM);
	// プロジェクション行列の更新
	projectionMatrix_ = MakePerspectiveMatrix(verticalFOV_, aspectRatio_, nearClip_, farClip_);
	// 上記２つをビュープロジェクション行列に合成
	viewprojectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);


}