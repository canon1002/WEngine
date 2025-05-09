#include "MultiModel.h"
#include "ModelCommon.h"
#include "../camera/MainCamera.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Resource/Texture/TextureManager.h"

MultiModel::~MultiModel() {
	CameraResource.Reset();
}

void MultiModel::Initialize(const std::string& directrypath, const std::string& filename){

	// モデル読み込み
	modelData = Resource::LoadMultiModelFile(directrypath, filename);
	// 頂点リソース 生成
	CreateVertexResource();
	// Indexリソース 生成
	CreateIndexResource();
	// マテリアルリソース 生成
	CreateMaterialResource();

}

void MultiModel::Update()
{

}

void MultiModel::Draw()
{
	// メッシュの数だけ実行する
	for (const Mesh& mesh : modelData.meshes) {
		const MultiMaterial& material = modelData.materials[mesh.materialIndex];

		// 配列を渡す(開始スロット番号、使用スロット数、VBV配列へのポインタ)
		DirectXCommon::GetInstance()->mCommandList->IASetVertexBuffers(0, 1, &mesh.vertexBufferView);
		// IndexBufferViewをセット
		DirectXCommon::GetInstance()->mCommandList->IASetIndexBuffer(&mesh.indexBufferView);
		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
		DirectXCommon::GetInstance()->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// マテリアルのCBufferの場所を指定
		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(0, material.materialResource->GetGPUVirtualAddress());
		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(4, CameraResource->GetGPUVirtualAddress());

		// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
		DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->mDefaultTextureIndex);
			//mCommandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->mTextureData.at(TextureManager::GetInstance()->mDefaultTexID).textureSrvHandleGPU);
		// インデックスを使用してドローコール
		DirectXCommon::GetInstance()->mCommandList->DrawIndexedInstanced(UINT(modelData.meshes[0].indices.size()), 1, 0, 0, 0);
	}
}

void MultiModel::DrawGUI(const std::string& label){
	label;
#ifdef _DEBUG
#endif // _DEBUG
}

//
void MultiModel::CreateVertexResource() {
	// メッシュの数だけ実行する
	for (Mesh& mesh : modelData.meshes) {
		// 実際に頂点リソースを作る
		mesh.vertexResource = DirectXCommon::GetInstance()->CreateBufferResource(
			DirectXCommon::GetInstance()->mDevice.Get(), sizeof(VertexData) * mesh.vertices.size());

		// リソースの先頭のアドレスから使う
		mesh.vertexBufferView.BufferLocation = mesh.vertexResource->GetGPUVirtualAddress();
		// 使用するリソースサイズは頂点分のサイズ
		mesh.vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * mesh.vertices.size());
		// 1頂点あたりのサイズ
		mesh.vertexBufferView.StrideInBytes = sizeof(VertexData);

		// 頂点リソースにデータを書き込む
		mesh.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mesh.vertices));// 書き込むためのアドレスを取得
		std::memcpy(&mesh.vertices, mesh.vertices.data(), sizeof(VertexData) * mesh.vertices.size());
	}
}

void MultiModel::CreateIndexResource(){
	// メッシュの数だけ実行する
	for (Mesh& mesh : modelData.meshes) {
		// Indexは <uint32_t * Indexデータのサイズ> 分だけ確保する
		mesh.indexResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(uint32_t) * mesh.indices.size());
		// GPUアドレスを取得
		mesh.indexBufferView.BufferLocation = mesh.indexResource->GetGPUVirtualAddress();
		// Byte数は <uint32_t * Indexデータのサイズ>分
		mesh.indexBufferView.SizeInBytes = sizeof(uint32_t) * (uint32_t)mesh.indices.size();
		mesh.indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		// Rsourceに対してIndexの内容をコピーする
		uint32_t* mappedIndex = nullptr;
		mesh.indexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex));
		std::memcpy(mappedIndex, mesh.indices.data(), sizeof(uint32_t) * mesh.indices.size());
	}
}

void MultiModel::CreateMaterialResource(){
	// メッシュの数だけ実行する
	for (const Mesh& mesh : modelData.meshes) {
		MultiMaterial& material = modelData.materials[mesh.materialIndex];

		// マテリアル用のResourceを作る
		material.materialResource = DirectXCommon::GetInstance()->
			CreateBufferResource(DirectXCommon::GetInstance()->
				mDevice.Get(), sizeof(Material));

		// 書き込むためのアドレスを取得
		material.materialResource->Map(0, nullptr, reinterpret_cast<void**>(&material.materialItem));

		// 色の書き込み・Lightingの無効化
		material.materialItem.color = { 1.0f, 1.0f, 1.0f, 1.0f };
		material.materialItem.enableLighting = true;
		// UVTransformを設定
		material.materialItem.uvTransform = MakeIdentity();
		uvTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
		material.materialItem.shininess = 100.0f;
	}

	// Light
	directionalLightResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(DirectionalLight));
	// データを書き込む
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDate));
	directionalLightDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightDate->direction = { 0.0f,-1.0f,0.0f };
	directionalLightDate->intensity = 1.0f;

	// カメラデータ
	CameraResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(CameraForGPU));
	// データを書き込む
	CameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));
	mCamera->Init();
	cameraData->worldPosition = mCamera->GetTranslate();

}
