#include "Triangle.h"
#include "../object/camera/MatrixCamera.h"

Triangle::Triangle() { this->Initialize(); }

Triangle::~Triangle()
{
	/*wvpResource->Release();
	materialResource->Release();
	vertexResource->Release();*/
}

void Triangle::Initialize() {

	dx_ = DirectXCommon::GetInstance();
	mainCamera_ = MatrixCamera::GetInstance();

	translate_.x = 0.0f;
	translate_.y = 0.0f;
	translate_.z = 0.0f;
	translate_.w = 1.0f;

	CreateVertexResource();
	CreateTransformationRsource();
	CreateBufferView();

}

void Triangle::Update() {

}

void Triangle::Draw() {

	*(wvpData) = mainCamera_->GetWorldViewProjection();

	vertexData[0].position = mainCamera_->GetNdcPos({
		(translate_.x + (-0.5f)),
		(translate_.y + (-0.5f)),
		(translate_.z + 0.0f),
		1.0f });
	vertexData[1].position = mainCamera_->GetNdcPos({
		translate_.x + (0.0f),
		translate_.y + (0.5f),
		translate_.z + 0.0f,
		1.0f });
	vertexData[2].position = mainCamera_->GetNdcPos({
		translate_.x + (0.5f),
		translate_.y + (-0.5f),
		translate_.z + 0.0f,
		1.0f });


	dx_->commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	dx_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// CBV設定

	// マテリアルのCBufferの場所を指定
	dx_->commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を指定
	dx_->commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	dx_->commandList->SetGraphicsRootDescriptorTable(2, dx_->textureSrvHandleGPU_);

	// インスタンス生成
	dx_->commandList->DrawInstanced(3, 1, 0, 0);

}

//
void Triangle::CreateVertexResource() {

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	vertexResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(VertexData) * 3);

	// マテリアル用のResourceを作る
	materialResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(VertexData));
	// マテリアルにデータを書き込む
	materialDate = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialDate));
	// 色を書き込む
	*materialDate = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

}

//
void Triangle::CreateTransformationRsource() {

	// Transformation用のResourceを作る
	wvpResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(Matrix4x4));
	// データを書き込む
	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	*wvpData = mainCamera_->GetWorldViewProjection();

}

//
void Triangle::CreateBufferView() {

	// VertexBufferViewを作成する(P.43)
	// 頂点バッファビューを作成する
	vertexBufferView = {};
	// リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 3;
	// 1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// Resourceにデータを書き込む

	// 書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// 左下
	vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	// 上
	vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.5f,0.0f };
	// 右下
	vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };

}

//
void Triangle::DrawBeginResource() {


}