#include "Model.h"
#include "ModelCommon.h"
#include "../camera/MainCamera.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Resource/Texture/TextureManager.h"

Model::~Model() {
	mVertexResource.Reset();
	mCameraResource.Reset();
}


void Model::Init(const std::string& directrypath, const std::string& filename){

	// キューブマップのテクスチャ
	mTextureHandleCubeMap = 0;

	// モデル読み込み
	mModelData = Resource::LoadModelFile(directrypath, filename);

	// 頂点リソース 生成
	CreateVertexResource();
	// Indexリソース 生成
	CreateIndexResource();
	// マテリアルリソース 生成
	CreateMaterialResource();

}

void Model::Update()
{

}

void Model::Draw()
{
	// 配列を渡す(開始スロット番号、使用スロット数、VBV配列へのポインタ)
	DirectXCommon::GetInstance()->mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
	// IndexBufferViewをセット
	DirectXCommon::GetInstance()->mCommandList->IASetIndexBuffer(&mIndexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	DirectXCommon::GetInstance()->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアルのCBufferの場所を指定
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(0, mMaterialResource->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(3, mDirectionalLightResource->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(4, mCameraResource->GetGPUVirtualAddress());

	// テクスチャをセット
	DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(2, mTextureHandle);
	// CueMapのテクスチャをセット
	if (mTextureHandleCubeMap != 0) {
		DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(5, mTextureHandleCubeMap);
	}

	// インデックスを使用してドローコール
	DirectXCommon::GetInstance()->mCommandList->DrawIndexedInstanced(UINT(mModelData.indices.size()), 1, 0, 0, 0);
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
	mVertexResource = DirectXCommon::GetInstance()->CreateBufferResource(
		DirectXCommon::GetInstance()->mDevice.Get(), sizeof(VertexData) * mModelData.vertices.size());

	// リソースの先頭のアドレスから使う
	mVertexBufferView.BufferLocation = mVertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点分のサイズ
	mVertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * mModelData.vertices.size());
	// 1頂点あたりのサイズ
	mVertexBufferView.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	mVertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mVertexData));// 書き込むためのアドレスを取得
	std::memcpy(mVertexData, mModelData.vertices.data(), sizeof(VertexData) * mModelData.vertices.size());

}

void Model::CreateIndexResource(){

	// Indexは <uint32_t * Indexデータのサイズ> 分だけ確保する
	mIndexResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(uint32_t) * mModelData.indices.size());
	// GPUアドレスを取得
	mIndexBufferView.BufferLocation = mIndexResource->GetGPUVirtualAddress();
	// Byte数は <uint32_t * Indexデータのサイズ>分
	mIndexBufferView.SizeInBytes = sizeof(uint32_t) * (uint32_t)mModelData.indices.size();
	mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	// Rsourceに対してIndexの内容をコピーする
	uint32_t* mappedIndex = nullptr;
	mIndexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex));
	std::memcpy(mappedIndex, mModelData.indices.data(), sizeof(uint32_t) * mModelData.indices.size());

}

void Model::CreateMaterialResource()
{
	// マテリアル用のResourceを作る
	mMaterialResource = DirectXCommon::GetInstance()->
		CreateBufferResource(DirectXCommon::GetInstance()->
			mDevice.Get(), sizeof(Material));

	// マテリアルにデータを書き込む
	mMaterialData = nullptr;
	// 書き込むためのアドレスを取得
	mMaterialResource->Map(0, nullptr, reinterpret_cast<void**>(&mMaterialData));
	// テクスチャの情報を転送
	if (mModelData.material.textureFilePath.empty()) {
		mTextureHandle = TextureManager::GetInstance()->mDefaultTextureIndex;
	}
	else {
		mTextureHandle = TextureManager::GetInstance()->LoadTexture(mModelData.material.textureFilePath);
	}
	// 色の書き込み・Lightingの無効化
	mMaterialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	mMaterialData->enableLighting = true;
	mMaterialData->shininess = 100.0f;
	// UVTransformを設定
	mMaterialData->uvTransform = MakeIdentity();
	mUvTransform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	

	// Light
	mDirectionalLightResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(DirectionalLight));
	// データを書き込む
	mDirectionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&mDirectionalLightData));
	mDirectionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	mDirectionalLightData->direction = { 0.0f,-1.0f,0.0f };
	mDirectionalLightData->intensity = 1.0f;

	// カメラデータ
	mCameraResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(CameraForGPU));
	// データを書き込む
	mCameraResource->Map(0, nullptr, reinterpret_cast<void**>(&mCameraData));
	mCameraData->worldPosition = MainCamera::GetInstance()->GetTranslate();

}
