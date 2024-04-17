#include "Cube.h"
#include "ModelCommon.h"

void Cube::Init()
{
	dx_->GetInstance();
	CreateVertexResource();
	CreateMaterialResource();
}

void Cube::Update()
{

}

void Cube::Draw()
{

	dx_->commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	dx_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// CBV設定

	// マテリアルのCBufferの場所を指定
	dx_->commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	
	dx_->commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	dx_->commandList->SetGraphicsRootDescriptorTable(2, dx_->srv_->textureData_.at(textureHandle_).textureSrvHandleGPU);

	// インデックスを使用してドローコール
	dx_->commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
}

//
void Cube::CreateVertexResource() {

	// モデル読み込み
	modelData = Resource::LoadObjFile("resources/objs", "plane.obj");

	// 実際に頂点リソースを作る
	vertexResource = dx_->CreateBufferResource(
		dx_->device_.Get(), sizeof(VertexData) * modelData.vertices.size());

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

void Cube::CreateMaterialResource()
{
	// マテリアル用のResourceを作る
	materialResource = dx_->
		CreateBufferResource(dx_->
			device_.Get(), sizeof(Material));

	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// テクスチャの情報を転送
	textureHandle_ = dx_->srv_->LoadTexture(modelData.material.textureFilePath);
	// 色の書き込み・Lightingの無効化
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = true;
	// UVTransformを設定
	materialData_->uvTransform = MakeIdentity();
	uvTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	// Light
	directionalLightResource = dx_->CreateBufferResource(dx_->device_.Get(), sizeof(DirectionalLight));
	// データを書き込む
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDate));
	directionalLightDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightDate->direction = { 0.0f,-1.0f,0.0f };
	directionalLightDate->intensity = 1.0f;
}
