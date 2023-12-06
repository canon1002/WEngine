#include "SRV.h"
#include "../resources/Section/Resource.h"
#include "../object/model/Model.h"
#include "../math/Math.h"


// staticメンバ変数で宣言したインスタンスを初期化
SRV* SRV::instance = nullptr;

// コンストラクタ
SRV::SRV(){}

// デストラクタ
SRV::~SRV(){}

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
	srvDescriptorHeap = dx_->CreateDescriptorHeap(dx_->device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	// ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(dx_->win_->hwnd);
	ImGui_ImplDX12_Init(dx_->device.Get(),
		dx_->swapChainDesc.BufferCount,
		dx_->rtvDesc.Format,
		srvDescriptorHeap.Get(),
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);

	SetSRVDesc0("Resources/texture/uvChecker.png");

}

void SRV::CreateSRVDescriptorHeap() {

}

void SRV::SetSRVDesc0(const char* filePath) {

	// テクスチャを読んで転送する
	DirectX::ScratchImage mipImages = Resource::LoadTextrue(filePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	textureResource_ = Resource::CreateTextureResource(dx_->device, metadata);
	intermediaResource = Resource::UpdateTextureData(textureResource_, mipImages);

	// metaDateを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// デスクリプタサイズを取得
	const uint32_t descriptorSizeSRV = dx_->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	const uint32_t descriptorSizeRTV = dx_->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	const uint32_t descriptorSizeDSV = dx_->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//SRVを制作するDescriptorHeapの場所を決める
	textureSrvHandleCPU_ = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	textureSrvHandleGPU_ = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	// 先頭はImGuiが使うのでその次を使う
	textureSrvHandleCPU_.ptr += dx_->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU_.ptr += dx_->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//SRVの生成
	dx_->device->CreateShaderResourceView(textureResource_.Get(), &srvDesc, textureSrvHandleCPU_);

}


void SRV::SetSRVDesc(const std::string filePath) {

	// 二枚目のテクスチャを読んで転送する
	DirectX::ScratchImage mipImages2 = Resource::LoadTextrue(filePath);
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	textureResource2_ = Resource::CreateTextureResource(dx_->device, metadata2);
	intermediaResource2 = Resource::UpdateTextureData(textureResource2_, mipImages2);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	// デスクリプタサイズを取得
	const uint32_t descriptorSizeSRV = dx_->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	const uint32_t descriptorSizeRTV = dx_->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	const uint32_t descriptorSizeDSV = dx_->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);


	textureSrvHandleCPU3_ = dx_->GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);
	textureSrvHandleGPU3_ = dx_->GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);

	// 二枚目
	textureSrvHandleCPU3_.ptr += dx_->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU3_.ptr += dx_->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	dx_->device->CreateShaderResourceView(textureResource2_.Get(), &srvDesc2, textureSrvHandleCPU3_);

}