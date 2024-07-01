#include "Object3d.h"
#include "Object3dCommon.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

Object3d::Object3d() {
	mObjname = "Object3D";
}

Object3d::Object3d(const std::string objname){
	// 名称が引数に入っていれば命名しておく
	mObjname = objname;
}

Object3d::~Object3d() {
	//delete mWvpData;
	//delete model_;
	delete mWorldTransform;
}


void Object3d::Init(std::string name) {

	// オブジェクトの名称を設定
	mObjname = name;

	mDxCommon = DirectXCommon::GetInstance();
	mModelManager = ModelManager::GetInstance();
	mWorldTransform = new WorldTransform();
	mWorldTransform->Init();
	CreateTransformationRsource();
}

void Object3d::Update() {

	MainCamera* camera = MainCamera::GetInstance();
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = camera->GetViewMatrix();
	// WVPにまとめる
	wvpM = camera->GetViewProjectionMatrix();
	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	mWvpData->WVP = Multiply(mWorldTransform->GetWorldMatrix(), wvpM);
	mWvpData->World = mWorldTransform->GetWorldMatrix();
	mWvpData->InverseTransposeWorld = Inverse(Transpose(mWorldTransform->GetWorldMatrix()));

	// 早期リターン
	if (mModel == nullptr) {
		return;
	}

	// モデルデータに存在するNodeのLocalMatrixを適用する
	/*mWvpData->WVP = Multiply(model_->modelData.rootNode.localMatrix, Multiply(mWorldTransform->GetWorldMatrix(), wvpM));
	mWvpData->World = Multiply(model_->modelData.rootNode.localMatrix, mWorldTransform->GetWorldMatrix());
	mWvpData->InverseTransposeWorld = Transpose(Inverse(mWvpData->World));*/

	mModel->Update();

	// -- アニメーションが設定されている場合 -- // 

	if (mSkinning != nullptr) {
		mSkinning->Update();
	}

}

void Object3d::Draw() {

	// 早期リターン
	if (mModel == nullptr) {
		return;
	}

	//wvp用のCBufferの場所を指定
	mDxCommon->commandList->SetGraphicsRootConstantBufferView(1, mWvpResource->GetGPUVirtualAddress());
	// 頂点をセット

	D3D12_VERTEX_BUFFER_VIEW vbvs[2]{};
	vbvs[0] = mModel->mVertexBufferView;// VertexDataのVBV
	if (mSkinning != nullptr) {

		vbvs[1] = mSkinning->GetSkinCluster().influenceBufferView_; // influenceのVBV

		// 配列を渡す(開始スロット番号、使用スロット数、VBV配列へのポインタ)
		mDxCommon->commandList->IASetVertexBuffers(0, 2, vbvs);
		mDxCommon->commandList->SetGraphicsRootDescriptorTable(6,
			mSkinning->GetSkinCluster().paletteSrvHandle_.second);
	}
	else {
		// 配列を渡す(開始スロット番号、使用スロット数、VBV配列へのポインタ)
		mDxCommon->commandList->IASetVertexBuffers(0, 1, &vbvs[0]);
	}

	mModel->Draw();

}

void Object3d::DrawGUI()
{
#ifdef _DEBUG
	ImGui::Begin(mObjname.c_str());
	ImGui::SliderAngle("RotateX", &mWorldTransform->rotation.x);
	ImGui::SliderAngle("RotateY", &mWorldTransform->rotation.y);
	ImGui::SliderAngle("RotateZ", &mWorldTransform->rotation.z);
	ImGui::DragFloat3("Scale", &mWorldTransform->scale.x, 0.05f, -10.0f, 10.0f);
	ImGui::DragFloat3("Rotate", &mWorldTransform->rotation.x, 0.01f, -6.28f, 6.28f);
	ImGui::DragFloat3("translate", &mWorldTransform->translation.x, 0.1f, -100.0f, 100.0f);
	if (ImGui::TreeNode("WVPData")) {
		if (ImGui::TreeNode("WVP")) {
			for (int i = 0; i < 4; ++i) {
				// Floatの4x4行列内の数値を表示
				ImGui::DragFloat4(("Row " + std::to_string(i)).c_str(), mWvpData->WVP.m[i]);
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("World")) {
			for (int i = 0; i < 4; ++i) {
				// Floatの4x4行列内の数値を表示
				ImGui::DragFloat4(("Row " + std::to_string(i)).c_str(), mWvpData->World.m[i]);
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("InverseTransWorld")) {
			for (int i = 0; i < 4; ++i) {
				// Floatの4x4行列内の数値を表示
				ImGui::DragFloat4(("Row " + std::to_string(i)).c_str(), mWvpData->InverseTransposeWorld.m[i]);
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	if (mModel != nullptr) {
		mModel->DrawGUI(mObjname + "Model");

		if (mSkinning != nullptr) {
			if (ImGui::TreeNode("Skinning")) {
				if (ImGui::Button("再生")) {
					mSkinning->Play();
				}
				if (ImGui::Button("停止")) {
					mSkinning->Pause();
				}

				if (ImGui::Button("忍び足")) {
					mSkinning->Init("human", "sneakWalk.gltf", GetModel()->modelData);
				}
				if (ImGui::Button("歩き")) {
					mSkinning->Init("human", "walk.gltf", GetModel()->modelData);
				}

				ImGui::TreePop();
			}
		}
	}
	ImGui::End();

#endif // _DEBUG
}

//
void Object3d::CreateTransformationRsource() {

	// Transformation用のResourceを作る
	mWvpResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(TransformationMatrix));
	// データを書き込む
	// 書き込むためのアドレスを取得
	mWvpResource->Map(0, nullptr, reinterpret_cast<void**>(&mWvpData));
	// 単位行列を書き込む
	mWvpData->WVP = MakeAffineMatrix(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
	mWvpData->World = MakeIdentity();
	mWvpData->InverseTransposeWorld = Inverse(MakeIdentity());

}

void Object3d::SetModel(const std::string& filepath)
{
	// モデルを検索してセット
	//mModelManager->LoadModel(filepath);
	mModel = mModelManager->FindModel(filepath);
}
