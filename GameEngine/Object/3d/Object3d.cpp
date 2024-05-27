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
		ImGui::TreePop();
	}
	if (model_ != nullptr) {
		model_->DrawGUI("Model");
	}
	ImGui::End();

	MainCamera* camera = MainCamera::GetInstance();
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = camera->GetViewMatrix();
	// WVPにまとめる
	wvpM = camera->GetViewProjectionMatrix();
	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	wvpData->WVP = Multiply(worldTransform_->GetWorldMatrix(), wvpM);
	wvpData->World = worldTransform_->GetWorldMatrix();
	wvpData->InverseTransposeWorld = Transpose(Inverse(worldTransform_->GetWorldMatrix()));
	
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

	if (skeleton_.joints.size() > 0) {
		// 時刻を進める 右の数値(60.0fはフレームレートに応じて変動させるようにしたい)
		animationTime_ += 1.0f / 60.0f;
		// 最後まで行ったら最初からリピート再生する(しなくてもいいし、フラグで変更しても良さそう)
		animationTime_ = std::fmod(animationTime_, model_->animation_.duration);
		// アニメーションの更新を行い、骨ごとのLocal情報を更新する
		Animations::ApplyAniation(skeleton_, model_->animation_, animationTime_);

		// Skeletonの情報を更新
		Animations::UpdateSkeleton(skeleton_);
		// Skeletonの情報を基に、SkinClusterのMatrixPaletteを更新する
		Animations::UpdateSkinCluster(skinCluster_, skeleton_);
	}
	if(false) {

		if (model_->animation_.nodeAnimations.size() == 0) {
			// ノードアニメーションのサイズが0なら未設定として扱い、早期リターンする
			return;
		}

		// 時刻を進める 右の数値(60.0fはフレームレートに応じて変動させるようにしたい)
		animationTime_ += 1.0f / 60.0f;
		// 最後まで行ったら最初からリピート再生する(しなくてもいいし、フラグで変更しても良さそう)
		animationTime_ = std::fmod(animationTime_, model_->animation_.duration);
		// rootNodeのアニメーションを取得
		NodeAnimation& rootAnimation = model_->animation_.nodeAnimations[model_->modelData.rootNode.name];

		// 指定時刻に応じた値を取得する
		Vector3 translation = Animations::CalculateValue(rootAnimation.translation, animationTime_);
		Quaternion rotation = Animations::CalculateValue(rootAnimation.rotation, animationTime_);
		Vector3 scale = Animations::CalculateValue(rootAnimation.scale, animationTime_);
		Matrix4x4 localMatrix = MakeAffineMatrix(scale, rotation, translation);

		// モデルデータに存在するNodeのLocalMatrixを適用する
		wvpData->WVP = Multiply(localMatrix, Multiply(worldTransform_->GetWorldMatrix(), wvpM));
		wvpData->World = Multiply(localMatrix, worldTransform_->GetWorldMatrix());
		wvpData->InverseTransposeWorld = Transpose(Inverse(wvpData->World));

	}

}

void Object3d::Draw() {

	// 早期リターン
	if (model_ == nullptr) {
		return;
	}

	//wvp用のCBufferの場所を指定
	dxCommon_->commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	// 頂点をセット
	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
		model_->vertexBufferView,// VertexDataのVBV
		skinCluster_.influenceBufferView // influenceのVBV
	};
	// 配列を渡す(開始スロット番号、使用スロット数、VBV配列へのポインタ)
	dxCommon_->commandList->IASetVertexBuffers(0, 2, vbvs);
	dxCommon_->commandList->SetGraphicsRootDescriptorTable(5,
		skinCluster_.paletteSrvHandle.second);

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
	wvpData->WVP = MakeAffineMatrix(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
	wvpData->World = MakeIdentity();
	wvpData->InverseTransposeWorld = Inverse(MakeIdentity());

}

void Object3d::SetModel(const std::string& filepath)
{
	// モデルを検索してセット
	//modelManager_->LoadModel(filepath);
	model_ = modelManager_->FindModel(filepath);

	if (model_ != nullptr) {
		// Skeletonを生成する
		skeleton_ = Animations::CreateSkeleton(model_->modelData.rootNode);
		// Skeletonを基にSkincClusterを生成する
		skinCluster_ = CreateSkinCluster(dxCommon_->device_,skeleton_,model_->modelData);

	}
}
