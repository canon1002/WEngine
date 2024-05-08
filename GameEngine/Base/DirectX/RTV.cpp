#include "RTV.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Math/Math.h"

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
	// RTVを3つ作るのでディスクリプタを3つ用意

	// まずは1つ目を作る
	rtvHandles[0] = rtvStartHandle;
	dxCommon_->device_->CreateRenderTargetView(dxCommon_->swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
	// 2つ目のディスクリプタハンドルを得る(手動で)
	rtvHandles[1].ptr = rtvHandles[0].ptr + dxCommon_->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// 2つ目を作る
	dxCommon_->device_->CreateRenderTargetView(dxCommon_->swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);

	// -- ３つ目 RenderTexture -- //

	// RenderTextureで初期化する際の色
	const Vector4 kRenderTargetColor = { 1.0f,0.0f,0.0f,1.0f };
	// ResnderTextureを生成する
	//auto renderTextureResource = CreateRenderTextureResource(
	//	dxCommon_->device_, dxCommon_->win_->kClientWidth,
	//	dxCommon_->win_->kClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTargetColor);
	//dxCommon_->device_->CreateRenderTargetView(renderTextureResource.Get(), &rtvDesc, rtvHandles[3]);

}

Microsoft::WRL::ComPtr<ID3D12Resource> RTV::CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device,
	uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor) {

	//Resourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.Format = format;

	// 利用するヒープの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;// VRAM上に生成

	// 色の設定
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = format;
	clearValue.Color[0] = clearColor.x;
	clearValue.Color[1] = clearColor.y;
	clearValue.Color[2] = clearColor.z;
	clearValue.Color[3] = clearColor.w;

	// Resourceの生成
	Microsoft::WRL::ComPtr <ID3D12Resource> resource = nullptr;
	HRESULT hr =
		device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			&clearValue,
			IID_PPV_ARGS(&resource)
	);
	// 成否判定
	assert(SUCCEEDED(hr));

	// 作成したら値を返す
	return resource;
}
