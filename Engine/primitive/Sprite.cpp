#include "Sprite.h"
#include "../object/camera/MainCamera.h"

Sprite::Sprite() {}

Sprite::~Sprite()
{
	delete wvpData;
	delete vertexData;
	delete materialData;
	
}

void Sprite::Initialize() {

	dx_ = DirectXCommon::GetInstance();

	CreateVertexResource();
	CreateIndexResource();
	CreateTransformationRsource();
	CreateBufferView();

	wvpResource->SetName(L"Sphere");
	materialResourceSprite->SetName(L"Sphere");
	vertexResource->SetName(L"Sphere");
	indexResource->SetName(L"Sphere");

}

void Sprite::Update() {

	/*ImGui::Begin("Sprite");
	ImGui::DragFloat3("Scale", &worldTransform_.scale.x);
	ImGui::DragFloat3("Rotate", &worldTransform_.rotate.x);
	ImGui::DragFloat3("Tranlate", &worldTransform_.translate.x);
	ImGui::Spacing();
	ImGui::DragFloat2("UVScale", &uvTransform_.scale.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat2("UVTranlate", &uvTransform_.translate.x, 0.01f, -10.0f, 10.0f);
	ImGui::SliderAngle("UVRotate", &uvTransform_.rotate.z);
	ImGui::ColorEdit4("Color", &materialData->color.r);
	ImGui::End();*/

	//　矩形のワールド行列
	worldTransform_.worldM = MakeAffineMatrix(
		worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);

	// カメラのワールド行列
	cameraM = MainCamera::GetInstance()->GetWorldMatrix();
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = Inverse(cameraM);
	// 正規化デバイス座標系(NDC)に変換(正射影行列をかける)
	projectM = MainCamera::GetInstance()->GetProjectionMatrix();
	// WVPにまとめる
	wvpM = Multiply(viewM, projectM);
	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	wvpData->WVP = Multiply(worldTransform_.worldM, wvpM);
	wvpData->World = worldTransform_.worldM;

	/// マテリアル・UVTransform
	Mat44 uvTransformMatrix = MakeAffineMatrix(
		uvTransform_.scale,
		{ 0.0f,0.0f,uvTransform_.rotate.z },
		uvTransform_.translate
	);
	// 変換したデータを代入する
	materialData->uvTransform = uvTransformMatrix;



}

void Sprite::Draw() {

	dx_->commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	dx_->commandList->IASetIndexBuffer(&indexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	dx_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// CBV設定

	// マテリアルのCBufferの場所を指定
	dx_->commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を指定
	dx_->commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	dx_->commandList->SetGraphicsRootConstantBufferView(4, CameraResource->GetGPUVirtualAddress());

	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	dx_->commandList->SetGraphicsRootDescriptorTable(2, dx_->srv_->textureData_.at(textureHandle_).textureSrvHandleGPU);


	// インデックスを使用してドローコール
	dx_->commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

}

// インデックスデータの生成
void Sprite::CreateIndexResource() {

}

//
void Sprite::CreateVertexResource() {

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	vertexResource = dx_->CreateBufferResource(dx_->device_.Get(), sizeof(VertexData) * 6);

	// マテリアル用のResourceを作る
	materialResourceSprite = dx_->CreateBufferResource(dx_->device_.Get(), sizeof(Material));
	// マテリアルにデータを書き込む
	materialData = nullptr;
	// テクスチャの情報を転送
	if (textureHandle_ == 0) {
		textureHandle_ = dx_->srv_->defaultTexId_;
	}
	// 書き込むためのアドレスを取得
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 色の書き込み・Lightingの無効化
	materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData->enableLighting = false;
	// UVTransformを設定
	materialData->uvTransform = MakeIdentity();
	uvTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	// カメラデータ
	CameraResource = dx_->CreateBufferResource(dx_->device_.Get(), sizeof(CameraForGPU));
	// データを書き込む
	CameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));
	cameraData->worldPosition = MainCamera::GetInstance()->GetTranslate();

}

//
void Sprite::CreateTransformationRsource() {

	// Transformation用のResourceを作る
	wvpResource = dx_->CreateBufferResource(dx_->device_.Get(), sizeof(TransformationMatrix));
	// データを書き込む
	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	wvpData->WVP = MainCamera::GetInstance()->GetViewProjectionMatrix();
	wvpData->World = MakeIdentity();
}

//
void Sprite::CreateBufferView() {

	// リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点6つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// Resourceにデータを書き込む

	// 書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	
	/// 1枚目
	//　左上
	vertexData[0].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,0.0f };
	vertexData[0].normal = { 0.0f,0.0f,-1.0f };
	//　右上
	vertexData[1].position = { 640.0f,0.0f,0.0f,1.0f };
	vertexData[1].texcoord = { 1.0f,0.0f };
	vertexData[1].normal = { 0.0f,0.0f,-1.0f };
	// 左下
	vertexData[2].position = { 0.0f,360.0f,0.0f,1.0f };
	vertexData[2].texcoord = { 0.0f,1.0f };
	vertexData[2].normal = { 0.0f,0.0f,-1.0f };
	// 右下
	vertexData[3].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexData[3].texcoord = { 1.0f,1.0f };
	vertexData[3].normal = { 0.0f,0.0f,-1.0f };

	indexResource = dx_->CreateBufferResource(dx_->device_.Get(), sizeof(uint32_t) * 6);
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// インデックスリソースにデータを書き込む
	uint32_t* indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
	indexData[3] = 1; indexData[4] = 3; indexData[5] = 2;
}
