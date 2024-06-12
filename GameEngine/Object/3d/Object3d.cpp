#include "Object3d.h"
#include "Object3dCommon.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

Object3d::Object3d() {
	objname_ = "Object3D";
}

Object3d::Object3d(const std::string objname){
	// 名称が引数に入っていれば命名しておく
	objname_ = objname;
}

Object3d::~Object3d() {
	//delete wvpData;
	//delete model_;
	delete worldTransform_;
}


void Object3d::Init(std::string name) {

	// オブジェクトの名称を設定
	objname_ = name;

	mDxCommon = DirectXCommon::GetInstance();
	modelManager_ = ModelManager::GetInstance();
	worldTransform_ = new WorldTransform();
	worldTransform_->Init();
	CreateTransformationRsource();
	// アニメーションデータを保有しているか
	isHavingAnimation_ = false;
}

void Object3d::Update() {

	MainCamera* camera = MainCamera::GetInstance();
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = camera->GetViewMatrix();
	// WVPにまとめる
	wvpM = camera->GetViewProjectionMatrix();
	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	wvpData->WVP = Multiply(worldTransform_->GetWorldMatrix(), wvpM);
	wvpData->World = worldTransform_->GetWorldMatrix();
	wvpData->InverseTransposeWorld = Inverse(Transpose(worldTransform_->GetWorldMatrix()));

	// 早期リターン
	if (model_ == nullptr) {
		return;
	}

	// モデルデータに存在するNodeのLocalMatrixを適用する
	/*wvpData->WVP = Multiply(model_->modelData.rootNode.localMatrix, Multiply(worldTransform_->GetWorldMatrix(), wvpM));
	wvpData->World = Multiply(model_->modelData.rootNode.localMatrix, worldTransform_->GetWorldMatrix());
	wvpData->InverseTransposeWorld = Transpose(Inverse(wvpData->World));*/

	model_->Update();

	// -- アニメーションが設定されている場合 -- // 

	if (model_->skinning_ != nullptr) {
		model_->skinning_->Update();
	}

}

void Object3d::Draw() {

	// 早期リターン
	if (model_ == nullptr) {
		return;
	}

	//wvp用のCBufferの場所を指定
	mDxCommon->commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	// 頂点をセット

	D3D12_VERTEX_BUFFER_VIEW vbvs[2]{};
	vbvs[0] = model_->mVertexBufferView;// VertexDataのVBV
	if (model_->skinning_ != nullptr) {

		vbvs[1] = model_->skinning_->GetSkinCluster().influenceBufferView_; // influenceのVBV

		// 配列を渡す(開始スロット番号、使用スロット数、VBV配列へのポインタ)
		mDxCommon->commandList->IASetVertexBuffers(0, 2, vbvs);
		mDxCommon->commandList->SetGraphicsRootDescriptorTable(5,
			model_->skinning_->GetSkinCluster().paletteSrvHandle_.second);
	}
	else {
		// 配列を渡す(開始スロット番号、使用スロット数、VBV配列へのポインタ)
		mDxCommon->commandList->IASetVertexBuffers(0, 1, &vbvs[0]);
	}

	model_->Draw();

}

void Object3d::DrawGUI()
{
#ifdef _DEBUG
	ImGui::Begin(objname_.c_str());
	ImGui::SliderAngle("RotateX", &worldTransform_->rotation.x);
	ImGui::SliderAngle("RotateY", &worldTransform_->rotation.y);
	ImGui::SliderAngle("RotateZ", &worldTransform_->rotation.z);
	ImGui::DragFloat3("Scale", &worldTransform_->scale.x, 0.05f, -10.0f, 10.0f);
	ImGui::DragFloat3("Rotate", &worldTransform_->rotation.x, 0.01f, -6.28f, 6.28f);
	ImGui::DragFloat3("translate", &worldTransform_->translation.x, 0.1f, -100.0f, 100.0f);
	if (ImGui::TreeNode("WVPData")) {
		if (ImGui::TreeNode("WVP")) {
			for (int i = 0; i < 4; ++i) {
				// Floatの4x4行列内の数値を表示
				ImGui::DragFloat4(("Row " + std::to_string(i)).c_str(), wvpData->WVP.m[i]);
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("World")) {
			for (int i = 0; i < 4; ++i) {
				// Floatの4x4行列内の数値を表示
				ImGui::DragFloat4(("Row " + std::to_string(i)).c_str(), wvpData->World.m[i]);
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("InverseTransWorld")) {
			for (int i = 0; i < 4; ++i) {
				// Floatの4x4行列内の数値を表示
				ImGui::DragFloat4(("Row " + std::to_string(i)).c_str(), wvpData->InverseTransposeWorld.m[i]);
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	if (model_ != nullptr) {
		model_->DrawGUI("Model");
	}
	ImGui::End();

#endif // _DEBUG
}

//
void Object3d::CreateTransformationRsource() {

	// Transformation用のResourceを作る
	wvpResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(TransformationMatrix));
	// データを書き込む
	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	wvpData->WVP = MakeAffineMatrix(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
	wvpData->World = MakeIdentity();
	wvpData->InverseTransposeWorld = Inverse(MakeIdentity());

}

void Object3d::SetModel(const std::string& filepath)
{
	// モデルを検索してセット
	//modelManager_->LoadModel(filepath);
	model_ = modelManager_->FindModel(filepath);

}
