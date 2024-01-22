#include "SRV.h"
#include "../resources/Section/Resource.h"
#include "../object/model/Model.h"
#include "../math/Math.h"


// staticメンバ変数で宣言したインスタンスを初期化
SRV* SRV::instance = nullptr;

// コンストラクタ
SRV::SRV() {}

// デストラクタ
SRV::~SRV() {}

// インスタンスを取得
SRV* SRV::GetInstance() {
	// 関数内staticは初めて通ったときのみ実行される
	if (instance == nullptr) {
		instance = new SRV;
	}
	return instance;
}

// 消去
void SRV::Relese() {
	// インスタンスを解放
	delete instance;
	// deleteあとにnullptrを代入する
	instance = nullptr;
}

void SRV::Initialize(DirectXCommon* dx) {
	dx_ = dx;

	CreateShaderResourceView();
}

/// シェーダーリソースビュー生成
void SRV::CreateShaderResourceView() {

	// STV用ディスクリプタヒープの生成
	// タンスの部分
	srvDescriptorHeap = dx_->CreateDescriptorHeap(dx_->device_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	// ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(dx_->win_->hwnd);
	ImGui_ImplDX12_Init(dx_->device_.Get(),
		dx_->swapChainDesc.BufferCount,
		dx_->rtvDesc.Format,
		srvDescriptorHeap.Get(),
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);

	defaultTexId_ = LoadTexture("Resources/texture/uvChecker.png");

}

void SRV::CreateSRVDescriptorHeap() {

}

int SRV::LoadTexture(const std::string filePath) {

	TextureData textureData;
	++textureId_;

	// テクスチャを読んで転送する
	DirectX::ScratchImage mipImages = Resource::LoadTextrue(filePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	textureData.textureResource = Resource::CreateTextureResource(dx_->device_, metadata);
	textureData.intermediaResource = Resource::UpdateTextureData(textureData.textureResource, mipImages);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// デスクリプタサイズを取得
	const uint32_t descriptorSizeSRV = dx_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	const uint32_t descriptorSizeRTV = dx_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	const uint32_t descriptorSizeDSV = dx_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	textureData.textureSrvHandleCPU = dx_->GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, textureId_);
	textureData.textureSrvHandleGPU = dx_->GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, textureId_);

	textureData_.insert(std::make_pair(textureId_, textureData));

	dx_->device_->CreateShaderResourceView(textureData.textureResource.Get(), &srvDesc, textureData.textureSrvHandleCPU);

	return textureId_;

}

int SRV::SetStructuredBuffer(int32_t kNumInstance, Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource) {

	++textureId_;

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
	const uint32_t descriptorSizeSRV = dx_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	instancingSrvHandleCPU = dx_->GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, textureId_);
	instancingSrvHandleGPU = dx_->GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, textureId_);
	dx_->device_->CreateShaderResourceView(instancingResource.Get(), &instancingSrvDesc, instancingSrvHandleCPU);

	return  textureId_;

}