#include "RTV.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"


void RTV::Initialize(DirectXCommon* dxCommon){
	// DirectXCommonのポインタを取得
	dxCommon_ = dxCommon;

	// レンダーターゲットを生成する
	CreateFinalRenderTargets();

}

/// レンダーターゲット生成
void RTV::CreateFinalRenderTargets() {

	// RTV用ディスクリプタヒープの生成
	rtvDescriptorHeap = dxCommon_->CreateDescriptorHeap(dxCommon_->device_, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	// RTVの設定
	rtvDesc;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2Dテクスチャとして書き込む
	// ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	// RTVを2つ作るのでディスクリプタを２つ用意

	// まずは1つ目を作る
	rtvHandles[0] = rtvStartHandle;
	dxCommon_->device_->CreateRenderTargetView(dxCommon_->swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
	// 2つ目のディスクリプタハンドルを得る(手動で)
	rtvHandles[1].ptr = rtvHandles[0].ptr + dxCommon_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// 2つ目を作る
	dxCommon_->device_->CreateRenderTargetView(dxCommon_->swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);

}