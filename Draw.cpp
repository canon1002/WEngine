#include "Draw.h"

#include "Math.h"
#include "matrixCamera.h"

//
ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {

	// デバッグレイヤーでエラーと警告を受け取る
#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif // _DEBUG
	IDXGIFactory7* dxgiFactory = nullptr;
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(hr));

	// 返す用の変数を宣言
	ID3D12Resource* result = nullptr;

	// VertexResourceを生成する(P.42)
	// 頂点リソース用のヒープ設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;// UploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResouceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResouceDesc.Width = sizeInBytes;// リソースのサイズ。今回はVector4を3頂点分
	// バッファの場合はこれらは1にする決まり
	vertexResouceDesc.Height = 1;
	vertexResouceDesc.DepthOrArraySize = 1;
	vertexResouceDesc.MipLevels = 1;
	vertexResouceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	vertexResouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResouceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&result));
	assert(SUCCEEDED(hr));

	return result;
}


Draw::Draw()
{

}

Draw::~Draw()
{

}

// インスタンスを取得
Draw* Draw::GetInstance() {
	static Draw instance;
	return &instance;
}

void Draw::Initialize(WinAPI* win, DirectX* dx, MatrixCamera* mainCamera) {

	win_ = win;
	dx_ = dx;
	mainCamera_ = mainCamera;
	CreateVertexResource();
	CreateTransformationRsource();
	CreateBufferView();
	
}

void Draw::Delete() {
	// リソース
	wvpResource->Release();
	materialResource->Release();
	vertexResource->Release();
}

void Draw::CreateVertexResource() {

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	vertexResource = CreateBufferResource(dx_->device, sizeof(Vector4) * 3);

	// マテリアル用のResourceを作る
	materialResource = CreateBufferResource(dx_->device, sizeof(Vector4));
	// マテリアルにデータを書き込む
	Vector4* materialDate = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialDate));
	// 今回は赤色に書き込む
	*materialDate = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

}

void Draw::CreateTransformationRsource() {

	// Transformation用のResourceを作る
	wvpResource = CreateBufferResource(dx_->device, sizeof(Matrix4x4));
	// データを書き込む
	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	*wvpData = mainCamera_->GetWorldViewProjection();

}

void Draw::CreateBufferView() {

	// VertexBufferViewを作成する(P.43)
	// 頂点バッファビューを作成する
	vertexBufferView = {};
	// リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(Vector4) * 3;
	// 1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(Vector4);

	// Resourceにデータを書き込む

	// 書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// 左下
	vertexData[0] = { -0.5f,-0.5f,0.0f,1.0f };
	// 上
	vertexData[1] = { 0.0f,0.5f,0.0f,1.0f };
	// 右下
	vertexData[2] = { 0.5f,-0.5f,0.0f,1.0f };

}

void Draw::DrawBegin() {

	dx_->commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	dx_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	/// CBV設定

	// マテリアルのCBufferの場所を指定
	dx_->commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を指定
	dx_->commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

}

void Draw::DrawEnd() {

}
