#include "RTV.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"

void RTV::Initialize(DirectXCommon* dxCommon){
	// DirectXCommonのポインタを取得
	mDxCommon = dxCommon;

	// レンダーターゲットを生成する
	CreateFinalRenderTargets();

}

/// レンダーターゲット生成
void RTV::CreateFinalRenderTargets() {

	// RTV用ディスクリプタヒープの生成
	rtvDescriptorHeap = mDxCommon->CreateDescriptorHeap(mDxCommon->device_, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3, false);

	// RTVの設定
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2Dテクスチャとして書き込む
	// ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	// RTVを3つ作るのでディスクリプタを3つ用意

	// まずは1つ目を作る
	rtvHandles[0] = rtvStartHandle;
	mDxCommon->device_->CreateRenderTargetView(mDxCommon->swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
	// 2つ目のディスクリプタハンドルを得る(手動で)
	rtvHandles[1].ptr = rtvHandles[0].ptr + mDxCommon->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// 2つ目を作る
	mDxCommon->device_->CreateRenderTargetView(mDxCommon->swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);

	// -- ３つ目 RenderTexture -- //

	// ResnderTextureを生成する
	renderTextureResource = Resource::CreateRenderTextureResource(
		mDxCommon->device_,
		mDxCommon->win_->kClientWidth,
		mDxCommon->win_->kClientHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		Vector4(kRenderTargetColor[0],kRenderTargetColor[1],kRenderTargetColor[2],kRenderTargetColor[3]));

	// 3つ目のディスクリプタハンドルを得る(手動で)
	rtvHandles[2].ptr = rtvHandles[1].ptr + mDxCommon->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	
	mDxCommon->device_->CreateRenderTargetView(
		renderTextureResource.Get(), &rtvDesc, rtvHandles[2]);

}
