#include "VoxelParticle.h"
#include "GameEngine/Object/Camera/MainCamera.h"


VoxelParticle::VoxelParticle() : randomEngine_(seedGenerator_()) {}

VoxelParticle::~VoxelParticle()
{
}

void VoxelParticle::Initialize(DirectXCommon* dxCommon,CameraCommon* camera) {

	dxCommon_ = dxCommon;
	camera_=camera;
	worldTransform_.scale = { 1.0f,1.0f,1.0f };
	worldTransform_.rotation = { 0.0f,0.0f,0.0f };
	worldTransform_.translation = { 0.0f,0.0f,0.0f };
	
	instanceCount_ = kNumMaxInstance;

	CreateVertexResource();
	CreateIndexResource();
	CreateTransformationRsource();
	CreateBufferView();
	instancingHandle_ = dxCommon_->srv_->SetStructuredBuffer(kNumMaxInstance, instancingResource);

	wvpResource->SetName(L"Voxel");
	materialResource->SetName(L"Voxel");
	vertexResource->SetName(L"Voxel");
	directionalLightResource->SetName(L"Voxel");
	instancingResource->SetName(L"Voxel");
}

void VoxelParticle::Update() {

	// Δtを定義 60fps固定してあるが、実時間を計測して可変fpsで動かせるようにしておきたい
	const float kDeltaTime = 1.0f / 60.0f;

	if (instanceCount_ > kNumMaxInstance) {
		instanceCount_ = kNumMaxInstance;
	}

	/*ImGui::Begin("Pirticle");
	float treeScale = worldTransform_.scale.x;
	ImGui::DragFloat("ObjScale", &treeScale, 0.05f);
	worldTransform_.scale = { treeScale ,treeScale ,treeScale };
	ImGui::SliderAngle("ObjRotateX", &worldTransform_.rotate.x);
	ImGui::SliderAngle("ObjRotateY", &worldTransform_.rotate.y);
	ImGui::SliderAngle("ObjRotateZ", &worldTransform_.rotate.z);
	ImGui::DragFloat3("ObjTranlate", &worldTransform_.translate.x);
	ImGui::Spacing();
	ImGui::DragFloat2("UVScale", &uvTransform_.scale.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat2("UVTranlate", &uvTransform_.translate.x, 0.01f, -10.0f, 10.0f);
	ImGui::SliderAngle("UVRotate", &uvTransform_.rotate.z);
	ImGui::ColorEdit4("Color", &materialData_->color.r);
	ImGui::End();*/

	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = camera_->GetViewMatrix();
	// 正規化デバイス座標系(NDC)に変換(正射影行列をかける)
	pespectiveM = camera_->GetProjectionMatrix();
	// WVPにまとめる
	wvpM = camera_->GetViewProjectionMatrix();
	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	wvpData->WVP = Multiply(worldTransform_.GetWorldMatrix(), wvpM);
	wvpData->World = worldTransform_.GetWorldMatrix();

	for (int32_t index = 0; index < kNumMaxInstance; ++index) {
		if (particles[index].lifeTime <= particles[index].currentTime) {
			continue;
		}

		// パーティクルの移動を行う
		particles[index].translation += particles[index].vel * kDeltaTime;
		// 経過時間の加算
		particles[index].currentTime += kDeltaTime;
		// 徐々に透明にする
		float alpha = 1.0f - (particles[index].currentTime / particles[index].lifeTime);
		
		//　ワールド行列
		worldTransform_.scale = particles[index].scale;
		worldTransform_.rotation = particles[index].rotation;
		worldTransform_.translation = particles[index].translation;

		instancingData_[index].WVP = Multiply(worldTransform_.GetWorldMatrix(), wvpM);
		instancingData_[index].World = worldTransform_.GetWorldMatrix();
		instancingData_[index].color = particles[index].color;
		instancingData_[index].color.a = alpha;

	}

	/// マテリアル・UVTransform
	Mat44 uvTransformMatrix = MakeAffineMatrix(
		uvTransform_.scale,
		{ 0.0f,0.0f,uvTransform_.rotation.z },
		uvTransform_.translation
	);
	// 変換したデータを代入する
	materialData_->uvTransform = uvTransformMatrix;

}

void VoxelParticle::Draw() {

	dxCommon_->commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	dxCommon_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// CBV設定

	// マテリアルのCBufferの場所を指定
	dxCommon_->commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

	//wvp用のCBufferの場所を指定
	dxCommon_->commandList->SetGraphicsRootDescriptorTable(1, dxCommon_->srv_->instancingSrvHandleGPU);

	// テクスチャ
	dxCommon_->commandList->SetGraphicsRootDescriptorTable(2, dxCommon_->srv_->textureData_.at(dxCommon_->srv_->defaultTexId_).textureSrvHandleGPU);

	// インデックスを使用してドローコール
	dxCommon_->commandList->DrawInstanced(6, instanceCount_, 0, 0);

}

// インデックスデータの生成
void VoxelParticle::CreateIndexResource() {

}

//
void VoxelParticle::CreateVertexResource() {

	modelData_.vertices.push_back(VertexData{ .position = {-1.0f,  1.0f,0.0f,1.0f},.texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = { 1.0f,  1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = {-1.0f, -1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = {-1.0f, -1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = { 1.0f,  1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = { 1.0f, -1.0f,0.0f,1.0f},.texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });


	instancingResource = dxCommon_->CreateBufferResource(
		dxCommon_->device_.Get(), sizeof(ParticleForGPU) * kNumMaxInstance);
	// 書き込むためのアドレスを取得
	instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
	// 単位行列を書き込む
	for (int32_t index = 0; index < kNumMaxInstance; ++index) {
		instancingData_[index].WVP = MakeIdentity();
		instancingData_[index].World = MakeIdentity();
		instancingData_[index].color = Color(1.0f, 1.0f, 1.0f, 0.0f);

	}


	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	vertexResource = dxCommon_->CreateBufferResource(dxCommon_->device_.Get(), sizeof(VertexData) * modelData_.vertices.size());

	// マテリアル用のResourceを作る
	materialResource = dxCommon_->CreateBufferResource(dxCommon_->device_.Get(), sizeof(Material));
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 色の書き込み・Lightingの無効化
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = true;
	// UVTransformを設定
	materialData_->uvTransform = MakeIdentity();
	uvTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	// Light
	directionalLightResource = dxCommon_->CreateBufferResource(dxCommon_->device_.Get(), sizeof(DirectionalLight));
	// データを書き込む
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDate));
	directionalLightDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightDate->direction = { 0.0f,-1.0f,0.0f };
	directionalLightDate->intensity = 1.0f;
}

//
void VoxelParticle::CreateTransformationRsource() {

	for (int32_t index = 0; index < instanceCount_; ++index) {
		particles[index] = MakeNewParticle(randomEngine_);
	}

	// Transformation用のResourceを作る
	wvpResource = dxCommon_->CreateBufferResource(dxCommon_->device_.Get(), sizeof(ParticleForGPU));
	// データを書き込む
	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	wvpData->WVP = camera_->GetViewProjectionMatrix();
	wvpData->World = MakeIdentity();
	wvpData->color = Color(1.0f, 1.0f, 1.0f, 1.0f);

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

Particle VoxelParticle::MakeNewParticle(std::mt19937& randomEngine) {

	// 返り値
	Particle particle;
	// 乱数の最小・最大値
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

	// SRT・移動量・色の設定
	particle.scale = { 1.0f,1.0f,1.0f };
	particle.rotation = { 0.0f,0.0f,0.0f };
	particle.translation = {distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particle.vel = {distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particle.color = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine),0.5f };

	// 乱数の最小・最大値
	std::uniform_real_distribution<float> distTime(-1.0f, 3.0f);
	// 生存時間の設定
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;

	return particle;
}