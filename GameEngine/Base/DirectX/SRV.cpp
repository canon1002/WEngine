#include "SRV.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Model/Model.h"
#include "GameEngine/Math/Math.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

// staticメンバ変数で宣言したインスタンスを初期化
SRV* SRV::instance = nullptr;

// インスタンスを取得
SRV* SRV::GetInstance() {
	// 関数内staticは初めて通ったときのみ実行される
	if (instance == nullptr) {
		instance = new SRV;
	}
	return instance;
}

// 消去
void SRV::Finalize() {
	
	srvDescriptorHeap.Reset();
	// インスタンスを解放
	delete instance;
	// deleteあとにnullptrを代入する
	instance = nullptr;
}

void SRV::Initialize(DirectXCommon* dx) {
	dxCommon_ = dx;

	CreateShaderResourceView();

	srvDescriptorHeap->SetName(L"srvDescriptorHeap");
}

/// シェーダーリソースビュー生成
void SRV::CreateShaderResourceView() {

	// STV用ディスクリプタヒープの生成
	// タンスの部分
	srvDescriptorHeap = dxCommon_->CreateDescriptorHeap(dxCommon_->device_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	defaultTexId_ = LoadTexture("Resources/texture/circleWhite.png");

}

void SRV::CreateSRVDescriptorHeap() {

}

int SRV::LoadTexture(const std::string filePath) {


	// 新たにデータを登録する
	TextureData textureData;
	textureData.filePath = filePath;
	++textureId_;

	// テクスチャを読んで転送する
	DirectX::ScratchImage mipImages = Resource::LoadTextrue(filePath);
	textureData.metadata = mipImages.GetMetadata();
	textureData.textureResource = Resource::CreateTextureResource(dxCommon_->device_, textureData.metadata);
	textureData.intermediaResource = Resource::UpdateTextureData(dxCommon_,textureData.textureResource, mipImages);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureData.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);

	// デスクリプタサイズを取得
	const uint32_t descriptorSizeSRV = dxCommon_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//const uint32_t descriptorSizeRTV = dx_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//const uint32_t descriptorSizeDSV = dx_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	textureData.textureSrvHandleCPU = dxCommon_->GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, textureId_);
	textureData.textureSrvHandleGPU = dxCommon_->GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, textureId_);

	textureData_.insert(std::make_pair(textureId_, textureData));

	dxCommon_->device_->CreateShaderResourceView(textureData.textureResource.Get(), &srvDesc, textureData.textureSrvHandleCPU);

	return textureId_;

}

int SRV::SetStructuredBuffer(int32_t kNumInstance, Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource) {

	++particleId_;

	// SRVの設定をおこなう
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags= D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumInstance;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);
	// デスクリプタサイズを取得
	const uint32_t descriptorSizeSRV = dxCommon_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	instancingSrvHandleCPU = dxCommon_->GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, particleId_);
	instancingSrvHandleGPU = dxCommon_->GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, particleId_);
	dxCommon_->device_->CreateShaderResourceView(instancingResource.Get(), &instancingSrvDesc, instancingSrvHandleCPU);

	return  particleId_;

}

const DirectX::TexMetadata& SRV::GetMetaData(uint32_t textureId)
{
	TextureData& textureData = textureData_.at(textureId);
	return textureData.metadata;
}