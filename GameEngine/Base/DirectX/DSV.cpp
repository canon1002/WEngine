#include "DSV.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"

void DSV::Initialize(DirectXCommon* dxCommon){
	// DirectXCommonのポインタを取得
	dxCommon_ = dxCommon;
	// ディープステンシルビューを生成
	CreateDepthStencilView();
}

void DSV::CreateDepthStencilView(){

	// リソース生成
	depthStencilTextureResource_ = Resource::CreateDeapStencilTextureResource(dxCommon_->device_, dxCommon_->win_->kClientWidth, dxCommon_->win_->kClientHeight);

	// DSV用のヒープでディスクリプタの数は1。DSVはシェーダー内で触るものではないので、ShaderVisibleはfalse
	dsvDescriptorHeap_ = dxCommon_->CreateDescriptorHeap(dxCommon_->device_, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	// DSVの設定
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	// DSVHeapの先頭にDSVをつくる
	dxCommon_->device_->CreateDepthStencilView(depthStencilTextureResource_.Get(),
		&dsvDesc, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());

	dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

}


