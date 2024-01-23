#include "Object3d.h"
#include "Object3dCommon.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include "../camera/MatrixCamera.h"
#include "Model.h"

Object3d::Object3d() {}

Object3d::~ Object3d() {}

void Object3d::Init() {

	dx_ = DirectXCommon::GetInstance();
	mainCamera_ = MatrixCamera::GetInstance();
	worldTransform_ = new WorldTransform;


	CreateTransformationRsource();

}

void Object3d::Update() {

	//　矩形のワールド行列
	worldTransform_->worldM = MakeAffineMatrix(
		worldTransform_->scale, worldTransform_->rotate, worldTransform_->translate);

	// カメラのワールド行列
	cameraM = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-5.0f });
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = Inverse(cameraM);
	// 正規化デバイス座標系(NDC)に変換(正射影行列をかける)
	pespectiveM = MakePerspectiveMatrix(0.45f, (1280.0f / 720.0f), 0.1f, 100.0f);
	// WVPにまとめる
	wvpM = Multiply(viewM, pespectiveM);
	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	wvpData->WVP = Multiply(worldTransform_->worldM, wvpM);
	wvpData->World = worldTransform_->worldM;

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
	wvpData->WVP = mainCamera_->GetWorldViewProjection();
	wvpData->World = MakeIdentity();
}
