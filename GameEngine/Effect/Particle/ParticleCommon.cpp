#include "ParticleCommon.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include  "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/GameMaster/Framerate.h"

void ParticleCommon::Init(){

	mDxCommon = DirectXCommon::GetInstance();
	mCamera = MainCamera::GetInstance();
	mWorldTransform.scale = { 1.0f,1.0f,1.0f };
	mWorldTransform.rotation = { 0.0f,0.0f,0.0f };
	mWorldTransform.translation = { 0.0f,0.0f,0.0f };

	instanceCount_ = kNumMaxInstance;

	// エミッター初期設定
	mEmitter = {};
	mEmitter.worldTransform.Init();
	mEmitter.count = 3;
	mEmitter.frequency = 1.5f;// 1.5秒ごとに発生
	mEmitter.frequencyTime = 0.0f;// 発生頻度用の時刻 0で初期化

	// リソースを生成
	CreateTransformation();
	CreateVertex();
	CreateIndex();
	CreateMaterial();
	CreateInstancing();

	mDxCommon->srv_->SetStructuredBuffer(kNumMaxInstance, mInstancingResource);
}


void ParticleCommon::CreateTransformation() {
	for (int32_t index = 0; index < instanceCount_; ++index) {
		//particles[index] = MakeNewParticle(randomEngine_);
	}
	// Transformation用のResourceを作る
	mWVPResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(ParticleForGPU));
	// 書き込むためのアドレスを取得
	mWVPResource->Map(0, nullptr, reinterpret_cast<void**>(&mWVPData));
	// 単位行列を書き込む
	mWVPData->WVP = mCamera->GetViewProjectionMatrix();
	mWVPData->World = MakeIdentity();
	mWVPData->color = Color(1.0f, 1.0f, 1.0f, 1.0f);

}

void ParticleCommon::CreateVertex() {

	// とりあえず四角形で表示しておく
	modelData_.vertices.push_back(VertexData{ .position = {-1.0f,  1.0f,0.0f,1.0f},.texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = { 1.0f,  1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = {-1.0f, -1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = {-1.0f, -1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = { 1.0f,  1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = { 1.0f, -1.0f,0.0f,1.0f},.texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });

	// 頂点リソースを作る
	mVertexResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(VertexData) * modelData_.vertices.size());
	// リソースの先頭のアドレスから使う
	mVertexBufferView.BufferLocation = mVertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点分のサイズ
	mVertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	// 1頂点あたりのサイズ
	mVertexBufferView.StrideInBytes = sizeof(VertexData);
	// 頂点リソースにデータを書き込む
	mVertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mVertexData));// 書き込むためのアドレスを取得
	std::memcpy(mVertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

}

void ParticleCommon::CreateIndex()
{

}

void ParticleCommon::CreateMaterial() {

	// マテリアル用のResourceを作る
	mMaterialResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(Material));
	// マテリアルにデータを書き込む
	mMaterialData = nullptr;
	// 書き込むためのアドレスを取得
	mMaterialResource->Map(0, nullptr, reinterpret_cast<void**>(&mMaterialData));
	// 色の書き込み・Lightingの無効化
	mMaterialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	mMaterialData->enableLighting = true;
	// UVTransformを設定
	mMaterialData->uvTransform = MakeIdentity();
	uvTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

}

void ParticleCommon::CreateInstancing() {

	// リソースを生成
	mInstancingResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(ParticleForGPU) * kNumMaxInstance);
	// 書き込むためのアドレスを取得
	mInstancingResource->Map(0, nullptr, reinterpret_cast<void**>(&mInstancingData));

	for (int32_t index = 0; index < kNumMaxInstance; ++index) {
		// 単位行列を書き込む
		mInstancingData[index].WVP = MakeIdentity();
		mInstancingData[index].World = MakeIdentity();
		// カラーは透明にしておく
		mInstancingData[index].color = Color(1.0f, 1.0f, 1.0f, 0.0f);
	}


}
