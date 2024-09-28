#include "IPostEffect.h"


IPostEffect::~IPostEffect(){
	// グラフィックパイプライン
	mGraphicsPipelineState.Reset();
	// ルートシグネチャー
	mRootSignature.Reset();
	// 頂点リソース
	mVertexResource.Reset();
	// 頂点データ
	delete mVertexData;
}


void IPostEffect::CreateVertexResource() {

	// リソース生成
	mVertexResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(VertexData) * 3);
	// 頂点バッファビューを作成
	mVertexBufferView = {};
	// リソースの先頭のアドレスから使う
	mVertexBufferView.BufferLocation = mVertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点3つ分のサイズ
	mVertexBufferView.SizeInBytes = sizeof(VertexData) * 3;
	// 1頂点あたりのサイズ
	mVertexBufferView.StrideInBytes = sizeof(VertexData);
	// Resourceにデータを書き込む
	// 書き込むためのアドレスを取得
	mVertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mVertexData));
	// 左下
	mVertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	mVertexData[0].texcoord = { 0.0f,1.0f };
	// 上
	mVertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	mVertexData[1].texcoord = { 0.5f,0.0f };
	// 右下
	mVertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	mVertexData[2].texcoord = { 1.0f,1.0f };
}
