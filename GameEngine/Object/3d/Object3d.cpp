#include "Object3d.h"
#include "Object3dCommon.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

Object3d::Object3d() {}

Object3d::~Object3d() {
	//delete wvpData;
	//delete model_;
	delete worldTransform_;
}


void Object3d::Init() {

	dxCommon_ = DirectXCommon::GetInstance();
	modelManager_ = ModelManager::GetInstance();
	worldTransform_ = new WorldTransform();

	CreateTransformationRsource();

}

void Object3d::Update() {

	ImGui::Begin("Object3D");
	ImGui::SliderAngle("RotateX", &worldTransform_->rotation.x);
	ImGui::SliderAngle("RotateY", &worldTransform_->rotation.y);
	ImGui::SliderAngle("RotateZ", &worldTransform_->rotation.z);
	ImGui::DragFloat3("Scale", &worldTransform_->scale.x);
	ImGui::DragFloat3("Rotate", &worldTransform_->rotation.x);
	ImGui::DragFloat3("translate", &worldTransform_->translation.x);
	ImGui::DragFloat("shininess", &model_->materialData_->shininess);
	ImGui::ColorEdit4("Color",&model_->materialData_->color.r);
	ImGui::End();

	MainCamera* camera = MainCamera::GetInstance();
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = camera->GetViewMatrix();
	// 正規化デバイス座標系(NDC)に変換(正射影行列をかける)
	pespectiveM = camera->GetProjectionMatrix();
	// WVPにまとめる
	wvpM = camera->GetViewProjectionMatrix();
	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	wvpData->WVP = Multiply(worldTransform_->GetWorldMatrix(), wvpM);
	wvpData->World = worldTransform_->GetWorldMatrix();

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
	dxCommon_->commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

	model_->Draw();

}

//
void Object3d::CreateTransformationRsource() {

	// Transformation用のResourceを作る
	wvpResource = dxCommon_->CreateBufferResource(dxCommon_->device_.Get(), sizeof(TransformationMatrix));
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
	//modelManager_->LoadModel(filepath);
	model_ = modelManager_->FindModel(filepath);
}
