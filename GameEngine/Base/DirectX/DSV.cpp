#include "DSV.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"

void DSV::Init() {

	// 深度バッファ生成
	CreateDepthBuffer();
	// ディープステンシルビューを生成
	CreateDepthStencilView();
}


void DSV::CreateDepthBuffer()
{
	// 深度バッファリソースの作成
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;


	// 深度バッファのリソースディスクリプタ
	D3D12_RESOURCE_DESC depthBufferDesc = {};
	depthBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthBufferDesc.Alignment = 0;
	depthBufferDesc.Width = WinApp::GetInstance()->kClientWidth; // ウィンドウの幅
	depthBufferDesc.Height = WinApp::GetInstance()->kClientHeight; // ウィンドウの高さ
	depthBufferDesc.DepthOrArraySize = 1;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// 深度バッファのクリア値
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	HRESULT hr;
	hr = DirectXCommon::GetInstance()->mDevice->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&depthBufferDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(&mDepthBuffer)
	);
	assert(SUCCEEDED(hr));

}

void DSV::CreateDepthStencilView() {

	// リソース生成
	mDepthStencilTextureResource = Resource::CreateDeapStencilTextureResource(DirectXCommon::GetInstance()->mDevice, WinApp::GetInstance()->kClientWidth, WinApp::GetInstance()->kClientHeight);

	// DSV用のヒープでディスクリプタの数は1。DSVはシェーダー内で触るものではないので、ShaderVisibleはfalse
	mDsvDescriptorHeap = DirectXCommon::GetInstance()->CreateDescriptorHeap(DirectXCommon::GetInstance()->mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	// DSVの設定
	mDsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// Format。基本的にはResourceに合わせる
	mDsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	// DSVHeapの先頭にDSVをつくる
	DirectXCommon::GetInstance()->mDevice->CreateDepthStencilView(mDepthStencilTextureResource.Get(),
		&mDsvDesc, mDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	mDsvHandle = mDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

}
