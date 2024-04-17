#include "Triangle.h"
#include "GameEngine/Object/Camera/MainCamera.h"

Triangle::Triangle() {}

Triangle::~Triangle()
{
	delete wvpData;
	delete vertexData;
	delete materialData;
}

void Triangle::Initialize(DirectXCommon* dxCommon,CameraCommon* camera) {

	dxCommon_ = dxCommon;
	camera_ = camera;


	CreateVertexResource();
	CreateTransformationRsource();
	CreateBufferView();

	wvpResource->SetName(L"Tri");
	materialResource->SetName(L"Tri");
	vertexResource->SetName(L"Tri");
	
}

void Triangle::Update() {

}

void Triangle::Draw() {
	
	// カメラのワールド行列
	cameraM = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-5.0f });
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = Inverse(cameraM);
	// 正規化デバイス座標系(NDC)に変換(正射影行列をかける)
	pespectiveM = MakePerspectiveMatrix(0.45f, (1280.0f / 720.0f), 0.1f, 100.0f);
	// WVPにまとめる
	wvpM = Multiply(viewM,pespectiveM);
	// 三角形のワールド行列とWVP行列を掛け合わした行列を代入
	*wvpData = Multiply(worldTransform_.GetWorldMatrix(), wvpM);

	dxCommon_->commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	dxCommon_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// CBV設定

	// マテリアルのCBufferの場所を指定
	dxCommon_->commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を指定
	dxCommon_->commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	dxCommon_->commandList->SetGraphicsRootDescriptorTable(2, dxCommon_->srv_->textureData_.at(1).textureSrvHandleGPU);

	// インスタンス生成
	dxCommon_->commandList->DrawInstanced(3, 1, 0, 0);

}

//
void Triangle::CreateVertexResource() {

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	vertexResource = dxCommon_->CreateBufferResource(dxCommon_->device_.Get(), sizeof(VertexData) * 3);

	// マテリアル用のResourceを作る
	materialResource = dxCommon_->CreateBufferResource(dxCommon_->device_.Get(), sizeof(VertexData));
	// マテリアルにデータを書き込む
	materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 色を書き込む
	*materialData = Color(1.0f, 1.0f, 1.0f, 1.0f);

}

//
void Triangle::CreateTransformationRsource() {

	// Transformation用のResourceを作る
	wvpResource = dxCommon_->CreateBufferResource(dxCommon_->device_.Get(), sizeof(Mat44));
	// データを書き込む
	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	*wvpData = camera_->GetViewProjectionMatrix();

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