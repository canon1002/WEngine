#include "Object3d.h"
#include "Object3dCommon.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include "../camera/MainCamera.h"
#include "Model.h"
#include "ModelManager.h"

Object3d::Object3d() {}

Object3d::~ Object3d() {
	//delete wvpData;
	//delete model_;
}

void Object3d::Init() {

	dx_ = DirectXCommon::GetInstance();
	camera_ = MainCamera::GetInstance();
	worldTransform_.scale = { 1.0f,1.0f,1.0f };
	worldTransform_.rotate = { 0.0f,0.0f,0.0f };
	worldTransform_.translate = { 0.0f,0.0f,0.0f };
	worldTransform_.worldM = MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);

	CreateTransformationRsource();

}

void Object3d::Update() {

	//ImGui::Begin("Sphere");
	//ImGui::SliderAngle("RotateX", &worldTransform_.rotate.x);
	//ImGui::SliderAngle("RotateY", &worldTransform_.rotate.y);
	//ImGui::SliderAngle("RotateZ", &worldTransform_.rotate.z);
	//ImGui::DragFloat3("Rotate", &worldTransform_.rotate.x);
	//ImGui::DragFloat3("translate", &worldTransform_.translate.x);
	//ImGui::DragFloat("shininess", &model_->materialData_->shininess);
	//ImGui::ColorEdit4("Color",&model_->materialData_->color.r);
	//ImGui::End();


	// カメラのワールド行列
	cameraM = camera_->GetWorldMatrix();
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = camera_->GetViewMatrix();
	// 正規化デバイス座標系(NDC)に変換(正射影行列をかける)
	pespectiveM = camera_->GetProjectionMatrix();
	// WVPにまとめる
	wvpM = camera_->GetViewProjectionMatrix(); 
	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	wvpData->WVP = Multiply(worldTransform_.worldM, wvpM);
	wvpData->World = worldTransform_.worldM;

	// 早期リターン
	if (model_ == nullptr) {
		return;
	}

	model_->Update();
	
}

void Object3d::Draw() {

	// 早期リターン
	if (model_ == nullptr) {
		return;
	}

	//wvp用のCBufferの場所を指定
	dx_->commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

	model_->Draw();

}

//
void Object3d::CreateTransformationRsource() {

	// Transformation用のResourceを作る
	wvpResource = dx_->CreateBufferResource(dx_->device_.Get(), sizeof(TransformationMatrix));
	// データを書き込む
	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	wvpData->WVP = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
	wvpData->World = MakeIdentity();
}

void Object3d::SetModel(const std::string& filepath)
{
	// モデルを検索してセット
	model_ = ModelManager::GetInstance()->FindModel(filepath);
}
