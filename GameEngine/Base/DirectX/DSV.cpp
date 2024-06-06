#include "DSV.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"

void DSV::Initialize(DirectXCommon* dxCommon){
	// DirectXCommonのポインタを取得
	mDxCommon = dxCommon;
	// ディープステンシルビューを生成
	CreateDepthStencilView();
}

void DSV::CreateDepthStencilView(){

	// リソース生成
	mDepthStencilTextureResource = Resource::CreateDeapStencilTextureResource(mDxCommon->device_, mDxCommon->win_->kClientWidth, mDxCommon->win_->kClientHeight);

	// DSV用のヒープでディスクリプタの数は1。DSVはシェーダー内で触るものではないので、ShaderVisibleはfalse
	mDsvDescriptorHeap = mDxCommon->CreateDescriptorHeap(mDxCommon->device_, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	// DSVの設定
	mDsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// Format。基本的にはResourceに合わせる
	mDsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	// DSVHeapの先頭にDSVをつくる
	mDxCommon->device_->CreateDepthStencilView(mDepthStencilTextureResource.Get(),
		&mDsvDesc, mDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	mDsvHandle = mDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

}


