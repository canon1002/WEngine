#include "Object3d.h"

#include <assert.h>
#include <fstream>
#include <sstream>
//#include "Object/Camera/MainCamera.h"
#include "Resource/Model/ModelManager.h"
#include "Editor/ImGui/ImGuiManager.h"

void Object3d::Init(std::string objectName)
{
	// オブジェクト名を設定
	mObjectName = objectName;

	// ワールド座標初期化
	mWorldTransform = WorldTransform();

	// モデルに渡すTransformリソースを初期化
	CreateTransformation();

}


void Object3d::Update() {

}

void Object3d::Draw(Camera camera) {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 引数で取得したカメラから行列を取得してワールド座標を計算
	mWvpData->WVP = Multiply(mWorldTransform.GetWorldMatrix(), camera.GetViewProjectionMatrix());
	mWvpData->World = mWorldTransform.GetWorldMatrix();
	mWvpData->InverseTransposeWorld = Inverse(Transpose(mWorldTransform.GetWorldMatrix()));

	// 早期リターン
	if (mModel == nullptr) {
		return;
	}

	//wvp用のCBufferの場所を指定
	dxCommon->mCommandList->SetGraphicsRootConstantBufferView(1, mWvpResource->GetGPUVirtualAddress());
	// 頂点をセット

	D3D12_VERTEX_BUFFER_VIEW vbvs[2]{};
	vbvs[0] = mModel->mVertexBufferView;// VertexDataのVBV
	if (mSkinning != nullptr && mSkinning->GetIsActive()) {

		vbvs[1] = mSkinning->GetSkinCluster().influenceBufferView_; // influenceのVBV

		// 配列を渡す(開始スロット番号、使用スロット数、VBV配列へのポインタ)
		dxCommon->mCommandList->IASetVertexBuffers(0, 2, vbvs);
		dxCommon->mCommandList->SetGraphicsRootDescriptorTable(6,
			mSkinning->GetSkinCluster().paletteSrvHandle_.second);
	}
	else {
		// 配列を渡す(開始スロット番号、使用スロット数、VBV配列へのポインタ)
		dxCommon->mCommandList->IASetVertexBuffers(0, 1, &vbvs[0]);
	}

	mModel->Draw(camera);

}


void Object3d::Debug() {

#ifdef _DEBUG
	ImGui::Begin(mObjectName.c_str());
	ImGui::SliderAngle("RotateX", &mWorldTransform.rotation.x);
	ImGui::SliderAngle("RotateY", &mWorldTransform.rotation.y);
	ImGui::SliderAngle("RotateZ", &mWorldTransform.rotation.z);
	ImGui::DragFloat3("Scale", &mWorldTransform.scale.x, 0.05f, -10.0f, 10.0f);
	ImGui::DragFloat3("Rotate", &mWorldTransform.rotation.x, 0.01f, -6.28f, 6.28f);
	ImGui::DragFloat3("translate", &mWorldTransform.translation.x, 0.1f, -100.0f, 100.0f);
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
	if (mCollider != nullptr) {
		if (ImGui::TreeNode("Collider")) {
			ImGui::SliderAngle("RotateX", &mCollider->GetWorld()->rotation.x);
			ImGui::SliderAngle("RotateY", &mCollider->GetWorld()->rotation.y);
			ImGui::SliderAngle("RotateZ", &mCollider->GetWorld()->rotation.z);
			ImGui::DragFloat3("Scale", &mCollider->GetWorld()->scale.x, 0.05f, -10.0f, 10.0f);
			ImGui::DragFloat3("Rotate", &mCollider->GetWorld()->rotation.x, 0.01f, -6.28f, 6.28f);
			ImGui::DragFloat3("translate", &mCollider->GetWorld()->translation.x, 0.1f, -100.0f, 100.0f);

			mCollider->DebugDraw("CollderModel");
			ImGui::TreePop();
		}
	}
	if (mModel != nullptr) {
		mModel->DrawGUI(mObjectName + "Model");

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

void Object3d::CreateTransformation() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// Transformation用のResourceを作る
	mWvpResource = dxCommon->CreateBufferResource(dxCommon->device_.Get(), sizeof(TransformationMatrix));

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
	mModel = ModelManager::GetInstance()->FindModel(filepath);
}
