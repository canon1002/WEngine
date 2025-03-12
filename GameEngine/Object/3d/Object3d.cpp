#include "Object3d.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

Object3d::Object3d(const std::string objname) {
	// 名称が引数に入っていれば命名しておく
	mObjname = objname;
}

Object3d::~Object3d() {}


void Object3d::Init(std::string name) {

	// オブジェクトの名称を設定
	mObjname = name;

	// ワールド座標 初期化
	mWorldTransform = std::make_unique<WorldTransform>();
	mWorldTransform->Init();
	CreateTransformation();
}

void Object3d::Update() {

	MainCamera* camera = MainCamera::GetInstance();

	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	mWvpData->WVP = Multiply(mWorldTransform->GetWorldMatrix(), camera->GetViewProjectionMatrix());
	mWvpData->World = mWorldTransform->GetWorldMatrix();
	mWvpData->InverseTransposeWorld = Inverse(Transpose(mWorldTransform->GetWorldMatrix()));

	// -- モデル関連処理 -- //

	// 早期リターン
	if (mModel == nullptr) { return; }
	// 更新
	mModel->Update();

	// アニメーションが設定されている場合
	if (mSkinning != nullptr) { mSkinning->Update(); }

}

void Object3d::Draw() {

	// 早期リターン
	if (mModel == nullptr) {
		return;
	}

	//wvp用のCBufferの場所を指定
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(1, mWvpResource->GetGPUVirtualAddress());
	// 頂点をセット

	D3D12_VERTEX_BUFFER_VIEW vbvs[2]{};
	vbvs[0] = mModel->mVertexBufferView;// VertexDataのVBV
	if (mSkinning != nullptr && mSkinning->GetIsActive()) {

		vbvs[1] = mSkinning->GetNowSkinCluster()->skinCluster.influenceBufferView_; // influenceのVBV

		// 配列を渡す(開始スロット番号、使用スロット数、VBV配列へのポインタ)
		DirectXCommon::GetInstance()->mCommandList->IASetVertexBuffers(0, 2, vbvs);
		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootDescriptorTable(6,
			mSkinning->GetNowSkinCluster()->skinCluster.paletteSrvHandle_.second);
	}
	else {
		// 配列を渡す(開始スロット番号、使用スロット数、VBV配列へのポインタ)
		DirectXCommon::GetInstance()->mCommandList->IASetVertexBuffers(0, 1, &vbvs[0]);
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
		mModel->DrawGUI(mObjname + "Model");

		if (mSkinning != nullptr) {
			if (ImGui::CollapsingHeader("Animation")) {

				std::string strNormalT = "MotionBlendingTime : " + std::to_string(mSkinning->GetMotionBlendingTime());
				ImGui::ProgressBar(mSkinning->GetMotionBlendingTime(), ImVec2(-1.0f, 0.0f), strNormalT.c_str());

				// Skinning
				mSkinning->DrawGui();
			}
		}
	}
	ImGui::End();

#endif // _DEBUG
}

void Object3d::DrawGuiTree()
{

#ifdef _DEBUG
	if (!ImGui::BeginMenu(mObjname.c_str()))
		return;

	ImGui::SliderAngle("RotateX", &mWorldTransform->rotation.x);
	ImGui::SliderAngle("RotateY", &mWorldTransform->rotation.y);
	ImGui::SliderAngle("RotateZ", &mWorldTransform->rotation.z);
	ImGui::DragFloat3("Scale", &mWorldTransform->scale.x, 0.05f, -10.0f, 10.0f);
	ImGui::DragFloat3("Rotate", &mWorldTransform->rotation.x, 0.01f, -6.28f, 6.28f);
	ImGui::DragFloat3("translate", &mWorldTransform->translation.x, 0.1f, -100.0f, 100.0f);
	
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
		mModel->DrawGUI(mObjname + "Model");

		if (mSkinning != nullptr) {
		
			if (ImGui::CollapsingHeader("Animation")) {

				std::string strNormalT = "MotionBlendingTime : " + std::to_string(mSkinning->GetMotionBlendingTime());
				ImGui::ProgressBar(mSkinning->GetMotionBlendingTime(), ImVec2(-1.0f, 0.0f), strNormalT.c_str());

				// Skinning
				mSkinning->DrawGui();
			}


		}
	}

	ImGui::EndMenu();


#endif // _DEBUG

}

//
void Object3d::CreateTransformation() {

	// Transformation用のResourceを作る
	mWvpResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(TransformationMatrix));
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
	mModel = ModelManager::GetInstance()->FindModelPtr(filepath);

	// モデル内にアニメーションがある場合はアニメーション及びスキンクラスターなどを生成
	if (0) {
		mSkinning;
	}

}

void Object3d::SetModelFullPath(const string& directryPath, const string& filePath){

	// モデルを検索してセット
	mModel = ModelManager::GetInstance()->FindModelPtr(directryPath+filePath);
	// モデル内にアニメーションがある場合はアニメーション及びスキンクラスターなどを生成
	if (!mModel->modelData.skinClusterData.empty()) {
		mSkinning = std::make_unique<Skinning>();
		mSkinning->Init(directryPath, filePath, mModel->modelData);
	}

}
