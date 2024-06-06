#include "Model.h"
#include "ModelCommon.h"
#include "../camera/MainCamera.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

Model::~Model() {
	vertexResource.Reset();
	CameraResource.Reset();
}

void Model::Initialize(DirectXCommon* dxCommon,CameraCommon* camera,const std::string& directrypath,const std::string& filename)
{
	mDxCommon = dxCommon;
	camera_ = camera;

	// モデル読み込み
	modelData = Resource::LoadModelFile(directrypath,filename);

	// 頂点リソース 生成
	CreateVertexResource();
	// Indexリソース 生成
	CreateIndexResource();
	// マテリアルリソース 生成
	CreateMaterialResource();

}

void Model::Initialize(const std::string& directrypath, const std::string& filename){

	mDxCommon = DirectXCommon::GetInstance();
	camera_ = MainCamera::GetInstance();
	

	// モデル読み込み
	modelData = Resource::LoadModelFile(directrypath, filename);

	CreateVertexResource();
	CreateMaterialResource();

}

void Model::Update()
{

}

void Model::Draw()
{
	// 配列を渡す(開始スロット番号、使用スロット数、VBV配列へのポインタ)
	mDxCommon->commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	// IndexBufferViewをセット
	mDxCommon->commandList->IASetIndexBuffer(&indexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	mDxCommon->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアルのCBufferの場所を指定
	mDxCommon->commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	mDxCommon->commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
	mDxCommon->commandList->SetGraphicsRootConstantBufferView(4, CameraResource->GetGPUVirtualAddress());

	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	mDxCommon->commandList->SetGraphicsRootDescriptorTable(2, mDxCommon->srv_->textureData_.at(textureHandle_).textureSrvHandleGPU);
	// インデックスを使用してドローコール
	mDxCommon->commandList->DrawIndexedInstanced(UINT(modelData.indices.size()), 1, 0, 0, 0);
}

void Model::DrawGUI(const std::string& label){
#ifdef _DEBUG

	/// デバック情報を描画
	ImGui::BeginChild(label.c_str());
	// マテリアル
	if (ImGui::TreeNode("マテリアル")) {
		ImGui::DragFloat4("Color", &materialData_->color.r,0.01f,0.0f,1.0f);
		ImGui::TreePop();// ノードを閉じる(この場合は "マテリアル" を閉じる)
	}
	if (ImGui::TreeNode("平行光源")) {
		ImGui::Checkbox("Lighting Flag",&isLighting_);
		// Lightingの設定を変更できるように
		materialData_->enableLighting = isLighting_;
		ImGui::DragFloat("Shininess", &materialData_->shininess, 0.05f, 0.0f, 1.0f);
		ImGui::DragFloat4("Color", &directionalLightDate->color.r);
		ImGui::DragFloat3("Directon", &directionalLightDate->direction.x,0.1f,0.0f,1.0f);
		ImGui::DragFloat("Intensity", &directionalLightDate->intensity, 0.1f, 0.0f, 1.0f);
		ImGui::TreePop();
	}
	if (skinning_ != nullptr) {
		if (ImGui::TreeNode("Skinning")) {
			if (ImGui::Button("再生")) {
				skinning_->Play();
			}
			if (ImGui::Button("停止")) {
				skinning_->Pause();
			}

			if(ImGui::Button("忍び足") ){
				skinning_->Init("human", "sneakWalk.gltf", modelData);
			}
			if (ImGui::Button("歩き")) {
				skinning_->Init("human", "walk.gltf", modelData);
			}

			ImGui::TreePop();
		}
	}
	
	ImGui::EndChild();

#endif // _DEBUG
}

//
void Model::CreateVertexResource() {


	// 実際に頂点リソースを作る
	vertexResource = mDxCommon->CreateBufferResource(
		mDxCommon->device_.Get(), sizeof(VertexData) * modelData.vertices.size());

	// リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点分のサイズ
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	// 1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));// 書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

}

void Model::CreateIndexResource(){

	// Indexは <uint32_t * Indexデータのサイズ> 分だけ確保する
	indexResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(uint32_t) * modelData.indices.size());
	// GPUアドレスを取得
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	// Byte数は <uint32_t * Indexデータのサイズ>分
	indexBufferView.SizeInBytes = sizeof(uint32_t) * (uint32_t)modelData.indices.size();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	// Rsourceに対してIndexの内容をコピーする
	uint32_t* mappedIndex = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex));
	std::memcpy(mappedIndex, modelData.indices.data(), sizeof(uint32_t) * modelData.indices.size());

}

void Model::CreateMaterialResource()
{
	// マテリアル用のResourceを作る
	materialResource = mDxCommon->
		CreateBufferResource(mDxCommon->
			device_.Get(), sizeof(Material));

	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// テクスチャの情報を転送
	if (modelData.material.textureFilePath.empty()) {
		textureHandle_ = mDxCommon->srv_->defaultTexId_;
	}
	else {
		textureHandle_ = mDxCommon->srv_->LoadTexture(modelData.material.textureFilePath);
	}
	// 色の書き込み・Lightingの無効化
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = true;
	// UVTransformを設定
	materialData_->uvTransform = MakeIdentity();
	uvTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	materialData_->shininess = 100.0f;

	// Light
	directionalLightResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(DirectionalLight));
	// データを書き込む
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDate));
	directionalLightDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightDate->direction = { 0.0f,-1.0f,0.0f };
	directionalLightDate->intensity = 1.0f;

	// カメラデータ
	CameraResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(CameraForGPU));
	// データを書き込む
	CameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));
	camera_->Initialize(mDxCommon->win_);
	cameraData->worldPosition = camera_->GetTranslate();

}
