#include "ShadowObject.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Resource/Model/ModelManager.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"


using namespace DirectX;


ShadowObject::ShadowObject() {
	mObjname = "Object3D";
}

ShadowObject::ShadowObject(const std::string objname) {
	// 名称が引数に入っていれば命名しておく
	mObjname = objname;
}

ShadowObject::~ShadowObject() {
	//delete mWvpData;
	//delete model_;
	delete mWorldTransform;
	delete mMappedScene;
}

void ShadowObject::Init(std::string name){

	// オブジェクトの名称を設定
	mObjname = name;

	mWorldTransform = new WorldTransform();
	mWorldTransform->Init();
	CreateTransformation();
	CreateSceneResource();

}

void ShadowObject::Update(){

	MainCamera* camera = MainCamera::GetInstance();
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = camera->GetViewMatrix();
	// WVPにまとめる
	wvpM = camera->GetViewProjectionMatrix();
	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	mWvpData->WVP = Multiply(mWorldTransform->GetWorldMatrix(), wvpM);
	mWvpData->World = mWorldTransform->GetWorldMatrix();
	mWvpData->InverseTransposeWorld = Inverse(Transpose(mWorldTransform->GetWorldMatrix()));

	// -- モデル関連処理 -- //

	// 早期リターン
	if (mModel == nullptr) { return; }
	// 更新
	mModel->Update();

}

void ShadowObject::Draw(){

	// 早期リターン
	if (mModel == nullptr) {
		return;
	}

	//wvp用のCBufferの場所を指定
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(1, mWvpResource->GetGPUVirtualAddress());
	// 頂点をセット
	// 配列を渡す(開始スロット番号、使用スロット数、VBV配列へのポインタ)
	DirectXCommon::GetInstance()->mCommandList->IASetVertexBuffers(0, 1, &mModel->mVertexBufferView);
	// シャドウ用のCBVを渡す
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(6,mSceneResource->GetGPUVirtualAddress());
	// 描画
	mModel->Draw();

}


void ShadowObject::DrawGUI()
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
	}
	ImGui::End();

#endif // _DEBUG
}


//
void ShadowObject::CreateTransformation() {

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

void ShadowObject::CreateSceneResource()
{
	mSceneResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(SceneMatrix));
	// データを書き込む
	// 書き込むためのアドレスを取得
	mSceneResource->Map(0, nullptr, reinterpret_cast<void**>(&mParallelLightVector));

	// 向きを設定
	mParallelLightVector = { 1, -1, 1 };

	// -- 影行列の取得 -- //

	// 平面
	XMFLOAT4 planeVec{ 0,1,0,0 };
	// 計算
	mMappedScene = new SceneMatrix;
	mMappedScene->shadow = XMMatrixShadow(
		XMLoadFloat4(&planeVec),
		-XMLoadFloat3(&mParallelLightVector));
}

void ShadowObject::SetModel(const std::string& filepath)
{
	// モデルを検索してセット
	//mModelManager->LoadModel(filepath);
	mModel = ModelManager::GetInstance()->FindModel(filepath);
}

