#include "SRV.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Model/Model.h"
#include "GameEngine/Math/Math.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"


void SRV::Initialize(DirectXCommon* dx) {
	mDxCommon = dx;

	CreateShaderResourceView();

	srvDescriptorHeap->SetName(L"srvDescriptorHeap");
}

/// シェーダーリソースビュー生成
void SRV::CreateShaderResourceView() {

	// STV用ディスクリプタヒープの生成
	// タンスの部分
	srvDescriptorHeap = mDxCommon->CreateDescriptorHeap(mDxCommon->device_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	//defaultTexId_ = LoadTexture("uvChecker.png");
	defaultTexId_ = LoadTexture("uvChecker.dds");

}

void SRV::CreateSRVDescriptorHeap() {

}

int SRV::LoadTexture(const std::string filePath) {


	// 新たにデータを登録する
	TextureData textureData;
	textureData.filePath = filePath;
	++mTextureId;

	// テクスチャを読んで転送する
	DirectX::ScratchImage mipImages = Resource::LoadTextrue(filePath);
	textureData.metadata = mipImages.GetMetadata();
	textureData.textureResource = Resource::CreateTextureResource(mDxCommon->device_, textureData.metadata);
	textureData.intermediaResource = Resource::UpdateTextureData(mDxCommon,textureData.textureResource, mipImages);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureData.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	if (textureData.metadata.IsCubemap()) {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;// unionがTextureCubeになったが、内部パラメータはTexture2Dと変わらない
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	}
	else {
		// 今まで通り2Dテクスチャの設定を行う
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);
	}
	// デスクリプタサイズを取得
	const uint32_t descriptorSizeSRV = mDxCommon->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//const uint32_t descriptorSizeRTV = dx_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//const uint32_t descriptorSizeDSV = dx_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	textureData.textureSrvHandleCPU = mDxCommon->GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, mTextureId);
	textureData.textureSrvHandleGPU = mDxCommon->GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, mTextureId);

	mTextureData.insert(std::make_pair(mTextureId, textureData));

	mDxCommon->device_->CreateShaderResourceView(textureData.textureResource.Get(), &srvDesc, textureData.textureSrvHandleCPU);

	return mTextureId;

}

int SRV::SetStructuredBuffer(int32_t kNumInstance, Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource) {

	// 新たにデータを登録する
	TextureData instaicingData;
	++mParticleId;

	// SRVの設定をおこなう
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags= D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumInstance;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);
	// デスクリプタサイズを取得
	const uint32_t descriptorSizeSRV = mDxCommon->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	instaicingData.textureSrvHandleCPU = mDxCommon->GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, mParticleId);
	instaicingData.textureSrvHandleGPU = mDxCommon->GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, mParticleId);
	mDxCommon->device_->CreateShaderResourceView(instancingResource.Get(), &instancingSrvDesc, instaicingData.textureSrvHandleCPU);

	mTextureData.insert(std::make_pair(mParticleId, instaicingData));

	return  mParticleId;

}

int32_t SRV::CreateRenderTextureSRV(ID3D12Resource* pResource){

	// 新たにデータを登録する
	TextureData textureData;
	++mTextureId;

	// SRVの設定をおこなう
	D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	renderTextureSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	renderTextureSrvDesc.Texture2D.MipLevels = 1;

	// デスクリプタサイズを取得
	const uint32_t descriptorSizeSRV = mDxCommon->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//const uint32_t descriptorSizeRTV = dx_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//const uint32_t descriptorSizeDSV = dx_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	textureData.textureSrvHandleCPU = mDxCommon->GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, mTextureId);
	textureData.textureSrvHandleGPU = mDxCommon->GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, mTextureId);

	mTextureData.insert(std::make_pair(mTextureId, textureData));

	// SRVの生成
	mDxCommon->device_->CreateShaderResourceView(pResource,
		&renderTextureSrvDesc, textureData.textureSrvHandleCPU);

	return mTextureId;

}

const DirectX::TexMetadata& SRV::GetMetaData(uint32_t textureId)
{
	TextureData& textureData = mTextureData.at(textureId);
	return textureData.metadata;
}

int32_t SRV::GetEmptyIndex(){
	// 新たにデータを登録する
	TextureData textureData;
	++mTextureId;

	// デスクリプタサイズを取得
	const uint32_t descriptorSizeSRV = mDxCommon->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// CPUとGPUを取得/確保しておく
	textureData.textureSrvHandleCPU = mDxCommon->GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, mTextureId);
	textureData.textureSrvHandleGPU = mDxCommon->GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, mTextureId);

	// 一応mapに追加しておく
	mTextureData.insert(std::make_pair(mTextureId, textureData));

	return mTextureId;
}

int32_t SRV::CreateDepthSRV(Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource)
{
	// 新たにデータを登録する
	TextureData textureData;
	++mTextureId;

	// SRVの設定をおこなう
	D3D12_SHADER_RESOURCE_VIEW_DESC depthTextureSrvDesc{};
	depthTextureSrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	depthTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	depthTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	depthTextureSrvDesc.Texture2D.MipLevels = 1;

	//D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	//dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	//dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	//dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	// デスクリプタサイズを取得
	const uint32_t descriptorSizeSRV = mDxCommon->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//const uint32_t descriptorSizeRTV = dx_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//const uint32_t descriptorSizeDSV = dx_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	textureData.textureSrvHandleCPU = mDxCommon->GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, mTextureId);
	textureData.textureSrvHandleGPU = mDxCommon->GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, mTextureId);

	mTextureData.insert(std::make_pair(mTextureId, textureData));

	// SRVの生成
	mDxCommon->device_->CreateShaderResourceView(depthStencilResource.Get(),
		&depthTextureSrvDesc, textureData.textureSrvHandleCPU);

	return mTextureId;
}

