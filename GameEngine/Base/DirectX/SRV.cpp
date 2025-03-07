#include "SRV.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Object/Model/Model.h"
#include "GameEngine/Math/Math.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"


void SRV::Init() {

	// デスクリプタヒープの生成
	mDescriptorHeap = DirectXCommon::GetInstance()->CreateDescriptorHeap(
		DirectXCommon::GetInstance()->mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount, true);

	// デスクリプタ1個分のサイズを取得して記録する
	mDescriptorSize = DirectXCommon::GetInstance()->mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


}

uint32_t SRV::Allocate() {

	// 上限値に達していたらassrt
	assert(mUseIndex < kMaxSRVCount);
	// returnする番号を一度記録
	int32_t index = mUseIndex;
	// 次回のために番号を1進める
	mUseIndex++;
	// 上で記録した番号を返す
	return index;
}

void SRV::CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT format, UINT mipLevels){

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(mipLevels);
	
	DirectXCommon::GetInstance()->mDevice->CreateShaderResourceView(
		pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));

}

void SRV::CreateSRVforTextureCubeMap(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT format){

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;// unionがTextureCubeになったが、内部パラメータはTexture2Dと変わらない
	srvDesc.TextureCube.MipLevels = UINT_MAX;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

	DirectXCommon::GetInstance()->mDevice->CreateShaderResourceView(
		pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));

}

void SRV::CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride){

	// SRVの設定をおこなう
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = numElements;
	srvDesc.Buffer.StructureByteStride = structureByteStride;
	
	DirectXCommon::GetInstance()->mDevice->CreateShaderResourceView(
		pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));

}

void SRV::SetGraphicsRootDescriptorTable(UINT rootParameterIndex, uint32_t srvIndex){
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootDescriptorTable(
		rootParameterIndex, GetGPUDescriptorHandle(srvIndex));
}

void SRV::PreDraw(){

	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { mDescriptorHeap.Get() };
	DirectXCommon::GetInstance()->mCommandList->SetDescriptorHeaps(1, descriptorHeaps);

}


int SRV::SetInstancingBuffer(int32_t kNumInstance, Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource) {
	// 新たにデータを登録する
	TextureData instaicingData;
	++mParticleId;

	// SRVの設定をおこなう
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumInstance;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);
	instaicingData.textureSrvHandleCPU = GetCPUDescriptorHandle(mParticleId);
	instaicingData.textureSrvHandleGPU = GetGPUDescriptorHandle(mParticleId);
	DirectXCommon::GetInstance()->mDevice->CreateShaderResourceView(instancingResource.Get(), &instancingSrvDesc, instaicingData.textureSrvHandleCPU);

	mTextureData.insert(std::make_pair(mParticleId, instaicingData));

	return  mParticleId;

}

int32_t SRV::CreateRenderTextureSRV(ID3D12Resource* pResource) {

	// 新たにデータを登録する
	TextureData textureData;
	++mUseIndex;

	// SRVの設定をおこなう
	D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	renderTextureSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	renderTextureSrvDesc.Texture2D.MipLevels = 1;

	textureData.textureSrvHandleCPU = GetCPUDescriptorHandle(mUseIndex);
	textureData.textureSrvHandleGPU = GetGPUDescriptorHandle(mUseIndex);

	mTextureData.insert(std::make_pair(mUseIndex, textureData));

	// SRVの生成
	DirectXCommon::GetInstance()->mDevice->CreateShaderResourceView(pResource,
		&renderTextureSrvDesc, textureData.textureSrvHandleCPU);

	return mUseIndex;

}

const DirectX::TexMetadata& SRV::GetMetaData(uint32_t textureId)
{
	TextureData& textureData = mTextureData.at(textureId);
	return textureData.metadata;
}

int32_t SRV::GetEmptyIndex() {
	// 新たにデータを登録する
	TextureData textureData;
	++mUseIndex;

	// CPUとGPUを取得/確保しておく
	textureData.textureSrvHandleCPU = GetCPUDescriptorHandle(mUseIndex);
	textureData.textureSrvHandleGPU = GetGPUDescriptorHandle(mUseIndex);

	// 一応mapに追加しておく
	mTextureData.insert(std::make_pair(mUseIndex, textureData));

	return mUseIndex;
}

int32_t SRV::CreateDepthSRV(Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource)
{
	// 新たにデータを登録する
	TextureData textureData;
	++mUseIndex;

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
	//const uint32_t descriptorSizeSRV = DirectXCommon::GetInstance()->mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//const uint32_t descriptorSizeRTV = dx_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//const uint32_t descriptorSizeDSV = dx_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	textureData.textureSrvHandleCPU = GetCPUDescriptorHandle(mUseIndex);
	textureData.textureSrvHandleGPU = GetGPUDescriptorHandle(mUseIndex);

	mTextureData.insert(std::make_pair(mUseIndex, textureData));

	// SRVの生成
	DirectXCommon::GetInstance()->mDevice->CreateShaderResourceView(depthStencilResource.Get(),
		&depthTextureSrvDesc, textureData.textureSrvHandleCPU);

	return mUseIndex;
}

