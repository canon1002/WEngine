#include "VoxelParticle.h"
#include "../object/camera/MatrixCamera.h"

VoxelParticle::VoxelParticle() {}

VoxelParticle::~VoxelParticle()
{
	/*wvpResource->Release();
	materialResource->Release();
	vertexResource->Release();*/
}

void VoxelParticle::Initialize() {

	dx_ = DirectXCommon::GetInstance();
	mainCamera_ = MatrixCamera::GetInstance();
	worldTransform_ = new WorldTransform;
	instanceCount_ = 10;

	CreateVertexResource();
	CreateIndexResource();
	CreateTransformationRsource();
	CreateBufferView();
	instancingHandle_ = dx_->srv_->SetStructuredBuffer(kNumInstance, instancingResource);
}

void VoxelParticle::Update() {

	ImGui::Begin("Pirticle");
	float treeScale = worldTransform_->scale.x;
	ImGui::DragFloat("ObjScale", &treeScale, 0.05f);
	worldTransform_->scale = { treeScale ,treeScale ,treeScale };
	ImGui::SliderAngle("ObjRotateX", &worldTransform_->rotate.x);
	ImGui::SliderAngle("ObjRotateY", &worldTransform_->rotate.y);
	ImGui::SliderAngle("ObjRotateZ", &worldTransform_->rotate.z);
	ImGui::DragFloat3("ObjTranlate", &worldTransform_->translate.x);
	ImGui::Spacing();
	ImGui::DragFloat2("UVScale", &uvTransform_.scale.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat2("UVTranlate", &uvTransform_.translate.x, 0.01f, -10.0f, 10.0f);
	ImGui::SliderAngle("UVRotate", &uvTransform_.rotate.z);
	ImGui::ColorEdit4("Color", &materialData_->color.x);
	ImGui::End();


	//　矩形のワールド行列
	worldTransform_->worldM = W::Math::MakeAffineMatrix(
		transforms[0].scale, transforms[0].rotate, transforms[0].translate);


	// カメラのワールド行列
	cameraM = W::Math::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-5.0f });
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = W::Math::Inverse(cameraM);
	// 正規化デバイス座標系(NDC)に変換(正射影行列をかける)
	pespectiveM = W::Math::MakePerspectiveMatrix(0.45f, (1280.0f / 720.0f), 0.1f, 100.0f);
	// WVPにまとめる
	wvpM = W::Math::Multiply(viewM, pespectiveM);
	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	wvpData->WVP = W::Math::Multiply(worldTransform_->worldM, wvpM);
	wvpData->World = worldTransform_->worldM;

	for (int32_t index = 0; index < kNumInstance; ++index) {

		//　矩形のワールド行列
		worldTransform_->worldM = W::Math::MakeAffineMatrix(
			transforms[index].scale, transforms[index].rotate, transforms[index].translate);

		instancingData_[index].WVP = W::Math::Multiply(worldTransform_->worldM, wvpM);
		instancingData_[index].World = worldTransform_->worldM;
	}

	/// マテリアル・UVTransform
	Matrix4x4 uvTransformMatrix = W::Math::MakeAffineMatrix(
		uvTransform_.scale,
		{ 0.0f,0.0f,uvTransform_.rotate.z },
		uvTransform_.translate
	);
	// 変換したデータを代入する
	materialData_->uvTransform = uvTransformMatrix;

}

void VoxelParticle::Draw() {

	dx_->commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	dx_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// CBV設定

	// マテリアルのCBufferの場所を指定
	dx_->commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

	//wvp用のCBufferの場所を指定
	dx_->commandList->SetGraphicsRootDescriptorTable(1, dx_->srv_->instancingSrvHandleGPU);

	// テクスチャ
	dx_->commandList->SetGraphicsRootDescriptorTable(2, dx_->srv_->textureData_.at(dx_->srv_->defaultTexId_).textureSrvHandleGPU);

	// インデックスを使用してドローコール
	dx_->commandList->DrawInstanced(6, instanceCount_, 0, 0);

}

// インデックスデータの生成
void VoxelParticle::CreateIndexResource() {

}

//
void VoxelParticle::CreateVertexResource() {

	modelData_;
	modelData_.vertices.push_back(VertexData{ .position = {-1.0f,  1.0f,0.0f,1.0f},.texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = { 1.0f,  1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = {-1.0f, -1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = {-1.0f, -1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = { 1.0f,  1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = { 1.0f, -1.0f,0.0f,1.0f},.texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });

	
	instancingResource = dx_->CreateBufferResource(
		dx_->device.Get(), sizeof(TransformationMatrix) * kNumInstance);
	// 書き込むためのアドレスを取得
	instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
	// 単位行列を書き込む
	for (int32_t index = 0; index < kNumInstance; ++index) {
		instancingData_[index].WVP = W::Math::MakeIdentity();
		instancingData_[index].World = W::Math::MakeIdentity();
	}


	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	vertexResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(VertexData) * modelData_.vertices.size());

	// マテリアル用のResourceを作る
	materialResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(Material));
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 色の書き込み・Lightingの無効化
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = true;
	// UVTransformを設定
	materialData_->uvTransform = W::Math::MakeIdentity();
	uvTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	// Light
	directionalLightResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(DirectionalLight));
	// データを書き込む
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDate));
	directionalLightDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightDate->direction = { 0.0f,-1.0f,0.0f };
	directionalLightDate->intensity = 1.0f;
}

//
void VoxelParticle::CreateTransformationRsource() {

	for (int32_t index = 0; index < kNumInstance; ++index) {
		transforms[index].scale = { 1.0f,1.0f,1.0f };
		transforms[index].rotate = { 0.0f,0.0f,0.0f };
		transforms[index].translate = { index * 0.1f,index * 0.1f,index * 0.1f };
	}

	// Transformation用のResourceを作る
	wvpResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(TransformationMatrix));
	// データを書き込む
	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	wvpData->WVP = mainCamera_->GetWorldViewProjection();
	wvpData->World = W::Math::MakeIdentity();

	

}

//
void VoxelParticle::CreateBufferView() {

	// リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点分のサイズ
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	// 1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// Resourceにデータを書き込む

	// 頂点リソースにデータを書き込む
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));// 書き込むためのアドレスを取得

	std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}
