#include "Model.h"
#include "ModelCommon.h"
#include "../camera/MainCamera.h"

Model::~Model() {
	vertexResource.Reset();
	CameraResource.Reset();
}

void Model::Initialize(DirectXCommon* dxCommon,CameraCommon* camera,const std::string& directrypath,const std::string& filename)
{
	dxCommon_ = dxCommon;
	camera_ = camera;

	// モデル読み込み
	modelData = Resource::LoadObjFile(directrypath,filename);

	CreateVertexResource();
	CreateMaterialResource();

}

void Model::Initialize(const std::string& directrypath, const std::string& filename){

	dxCommon_ = DirectXCommon::GetInstance();
	camera_ = MainCamera::GetInstance();
	

	// モデル読み込み
	modelData = Resource::LoadObjFile(directrypath, filename);

	CreateVertexResource();
	CreateMaterialResource();

}

void Model::Update()
{

}

void Model::Draw()
{

	dxCommon_->commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	dxCommon_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// CBV設定

	// マテリアルのCBufferの場所を指定
	dxCommon_->commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

	dxCommon_->commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
	dxCommon_->commandList->SetGraphicsRootConstantBufferView(4, CameraResource->GetGPUVirtualAddress());

	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	dxCommon_->commandList->SetGraphicsRootDescriptorTable(2, dxCommon_->srv_->textureData_.at(textureHandle_).textureSrvHandleGPU);

	// インデックスを使用してドローコール
	dxCommon_->commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
}

//
void Model::CreateVertexResource() {


	// 実際に頂点リソースを作る
	vertexResource = dxCommon_->CreateBufferResource(
		dxCommon_->device_.Get(), sizeof(VertexData) * modelData.vertices.size());

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

void Model::CreateMaterialResource()
{
	// マテリアル用のResourceを作る
	materialResource = dxCommon_->
		CreateBufferResource(dxCommon_->
			device_.Get(), sizeof(Material));

	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// テクスチャの情報を転送
	if (modelData.material.textureFilePath.empty()) {
		textureHandle_ = dxCommon_->srv_->defaultTexId_;
	}
	else {
		textureHandle_ = dxCommon_->srv_->LoadTexture(modelData.material.textureFilePath);
	}
	// 色の書き込み・Lightingの無効化
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = true;
	// UVTransformを設定
	materialData_->uvTransform = MakeIdentity();
	uvTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	materialData_->shininess = 100.0f;

	// Light
	directionalLightResource = dxCommon_->CreateBufferResource(dxCommon_->device_.Get(), sizeof(DirectionalLight));
	// データを書き込む
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDate));
	directionalLightDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightDate->direction = { 0.0f,-1.0f,0.0f };
	directionalLightDate->intensity = 1.0f;

	// カメラデータ
	CameraResource = dxCommon_->CreateBufferResource(dxCommon_->device_.Get(), sizeof(CameraForGPU));
	// データを書き込む
	CameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));
	camera_->Initialize(dxCommon_->win_);
	cameraData->worldPosition = camera_->GetTranslate();

}
