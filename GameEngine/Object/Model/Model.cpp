#include "Model.h"
#include "ModelCommon.h"
#include "../camera/MainCamera.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

Model::~Model() {
	mVertexResource.Reset();
	mCameraResource.Reset();
}

void Model::Initialize(DirectXCommon* dxCommon,CameraCommon* camera,const std::string& directrypath,const std::string& filename)
{
	mDxCommon = dxCommon;
	mCamera = camera;

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
	mCamera = MainCamera::GetInstance();
	
	mTextureHandleCubeMap = 0;

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
	mDxCommon->mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
	// IndexBufferViewをセット
	mDxCommon->mCommandList->IASetIndexBuffer(&mIndexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	mDxCommon->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアルのCBufferの場所を指定
	mDxCommon->mCommandList->SetGraphicsRootConstantBufferView(0, mMaterialResource->GetGPUVirtualAddress());
	mDxCommon->mCommandList->SetGraphicsRootConstantBufferView(3, mDirectionalLightResource->GetGPUVirtualAddress());
	mDxCommon->mCommandList->SetGraphicsRootConstantBufferView(4, mCameraResource->GetGPUVirtualAddress());

	// テクスチャをセット
	mDxCommon->mCommandList->SetGraphicsRootDescriptorTable(2, mDxCommon->srv_->textureData_.at(mTextureHandle).textureSrvHandleGPU);
	// CueMapのテクスチャをセット
	if (mTextureHandleCubeMap != 0) {
		mDxCommon->mCommandList->SetGraphicsRootDescriptorTable(5, mDxCommon->srv_->textureData_.at(mTextureHandleCubeMap).textureSrvHandleGPU);
	}

	// インデックスを使用してドローコール
	mDxCommon->mCommandList->DrawIndexedInstanced(UINT(modelData.indices.size()), 1, 0, 0, 0);
}

void Model::DrawGUI(const std::string& label){
	label;
#ifdef _DEBUG
	if (ImGui::TreeNode(label.c_str())) {
		// マテリアル
		if (ImGui::TreeNode("マテリアル")) {
			ImGui::DragFloat4("Color", &mMaterialData->color.r, 0.01f, 0.0f, 1.0f);
			ImGui::TreePop();// ノードを閉じる(この場合は "マテリアル" を閉じる)
		}
		if (ImGui::TreeNode("平行光源")) {
			ImGui::Checkbox("Lighting Flag", &mIsLighting);
			// Lightingの設定を変更できるように
			mMaterialData->enableLighting = mIsLighting;
			ImGui::DragFloat("Shininess", &mMaterialData->shininess, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat4("Color", &mDirectionalLightData->color.r);
			ImGui::DragFloat3("Directon", &mDirectionalLightData->direction.x, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("Intensity", &mDirectionalLightData->intensity, 0.1f, 0.0f, 1.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("環境マップ")) {
			ImGui::DragFloat("EnvironmentCoefficient", &mMaterialData->environmentCoefficient, 0.01f, 0.0f, 1.0f);
			ImGui::TreePop();// ノードを閉じる(この場合は "マテリアル" を閉じる)
		}

		ImGui::TreePop();
	}

#endif // _DEBUG
}

//
void Model::CreateVertexResource() {


	// 実際に頂点リソースを作る
	mVertexResource = mDxCommon->CreateBufferResource(
		mDxCommon->device_.Get(), sizeof(VertexData) * modelData.vertices.size());

	// リソースの先頭のアドレスから使う
	mVertexBufferView.BufferLocation = mVertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点分のサイズ
	mVertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	// 1頂点あたりのサイズ
	mVertexBufferView.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	mVertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mVertexData));// 書き込むためのアドレスを取得
	std::memcpy(mVertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

}

void Model::CreateIndexResource(){

	// Indexは <uint32_t * Indexデータのサイズ> 分だけ確保する
	mIndexResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(uint32_t) * modelData.indices.size());
	// GPUアドレスを取得
	mIndexBufferView.BufferLocation = mIndexResource->GetGPUVirtualAddress();
	// Byte数は <uint32_t * Indexデータのサイズ>分
	mIndexBufferView.SizeInBytes = sizeof(uint32_t) * (uint32_t)modelData.indices.size();
	mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	// Rsourceに対してIndexの内容をコピーする
	uint32_t* mappedIndex = nullptr;
	mIndexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex));
	std::memcpy(mappedIndex, modelData.indices.data(), sizeof(uint32_t) * modelData.indices.size());

}

void Model::CreateMaterialResource()
{
	// マテリアル用のResourceを作る
	mMaterialResource = mDxCommon->
		CreateBufferResource(mDxCommon->
			device_.Get(), sizeof(Material));

	// マテリアルにデータを書き込む
	mMaterialData = nullptr;
	// 書き込むためのアドレスを取得
	mMaterialResource->Map(0, nullptr, reinterpret_cast<void**>(&mMaterialData));
	// テクスチャの情報を転送
	if (modelData.material.textureFilePath.empty()) {
		mTextureHandle = mDxCommon->srv_->defaultTexId_;
	}
	else {
		mTextureHandle = mDxCommon->srv_->LoadTexture(modelData.material.textureFilePath);
	}
	// 色の書き込み・Lightingの無効化
	mMaterialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	mMaterialData->enableLighting = true;
	// UVTransformを設定
	mMaterialData->uvTransform = MakeIdentity();
	mUvTransform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	mMaterialData->shininess = 100.0f;

	// Light
	mDirectionalLightResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(DirectionalLight));
	// データを書き込む
	mDirectionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&mDirectionalLightData));
	mDirectionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	mDirectionalLightData->direction = { 0.0f,-1.0f,0.0f };
	mDirectionalLightData->intensity = 1.0f;

	// カメラデータ
	mCameraResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(CameraForGPU));
	// データを書き込む
	mCameraResource->Map(0, nullptr, reinterpret_cast<void**>(&mCameraData));
	mCamera->Initialize();
	mCameraData->worldPosition = mCamera->GetTranslate();

}
