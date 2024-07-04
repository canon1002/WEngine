#include "AABBCollider.h"
#include "SphereCollider.h"
#include "GameEngine/Object/Camera/MainCamera.h"

AABBCollider::AABBCollider(WorldTransform* worldtransform, Vector3 radius) {
	// ワールド座標のポインタを代入
	pWorldTransform = worldtransform;
	// 各ベクトルの半径を代入
	mRadius = radius;
}

bool AABBCollider::IsCollision(Collider* c) {
	if (c->IsCollision(this)) {
		return true;
	}
	return false;
}

bool AABBCollider::IsCollision(AABBCollider* c)
{
	// AABB同士の判定を行う
	if ((this->GetMin().x <= c->GetMax().x && this->GetMax().x >= c->GetMin().x) &&
		(this->GetMin().y <= c->GetMax().y && this->GetMax().y >= c->GetMin().y) &&
		(this->GetMin().z <= c->GetMax().z && this->GetMax().z >= c->GetMin().z)) {

		return true;
	}

	return false;
}

bool AABBCollider::IsCollision(SphereCollider* c){
	
	Vector3 cloosestPoint{
		std::clamp(c->GetWorldPos().x,this->GetMin().x,this->GetMax().x),
		std::clamp(c->GetWorldPos().y,this->GetMin().y,this->GetMax().y),
		std::clamp(c->GetWorldPos().z,this->GetMin().z,this->GetMax().z)
	};

	float distance = Length(Subtract(cloosestPoint, c->GetWorldPos()));

	if (distance < c->GetRadius()) {
		return true;
	}
	return false;
}

Vector3 AABBCollider::GetMin()const {
	// AABBの最小値を計算する
	Vector3 min = {};
	// ワールド座標 - 各ベクトルの半径
	min = pWorldTransform->translation - mRadius;
	return min;
}

Vector3 AABBCollider::GetMax()const {
	// AABBの最大値を計算する
	Vector3 max = {};
	// ワールド座標 + 各ベクトルの半径
	max = pWorldTransform->translation + mRadius;
	return max;
}

void AABBCollider::Init(){
	mDxCommon = DirectXCommon::GetInstance();
	CreatePSO();
	CreateTransformation();
	CreateVertex();
	CreateIndex();
}

void AABBCollider::Update(){
	MainCamera* camera = MainCamera::GetInstance();
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = camera->GetViewMatrix();
	// WVPにまとめる
	wvpM = camera->GetViewProjectionMatrix();
	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	mWvpData->WVP = Multiply(pWorldTransform->GetWorldMatrix(), wvpM);
	mWvpData->World = pWorldTransform->GetWorldMatrix();
}

void AABBCollider::Draw(){

	// RootSignatureを設定。PSOに設定しているが、別途設定が必要
	mDxCommon->commandList->SetGraphicsRootSignature(mRootSignature.Get());
	mDxCommon->commandList->SetPipelineState(mGraphicsPipelineState.Get());

	//wvp用のCBufferの場所を指定
	mDxCommon->commandList->SetGraphicsRootConstantBufferView(0, mWvpResource->GetGPUVirtualAddress());
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	mDxCommon->commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	// 配列を渡す(開始スロット番号、使用スロット数、VBV配列へのポインタ)
	mDxCommon->commandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
	// IndexBufferViewをセット
	mDxCommon->commandList->IASetIndexBuffer(&mIndexBufferView);
	// インデックスを使用してドローコール
	mDxCommon->commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);

}

void AABBCollider::CreateTransformation(){
	// Transformation用のResourceを作る
	mWvpResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(TransformationMatrixForGrid3D));
	// データを書き込む
	// 書き込むためのアドレスを取得
	mWvpResource->Map(0, nullptr, reinterpret_cast<void**>(&mWvpData));
	// 単位行列を書き込む
	mWvpData->WVP = MakeAffineMatrix(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
	mWvpData->World = MakeIdentity();
}

void AABBCollider::CreateVertex(){
	// 実際に頂点リソースを作る
	mVertexResource = mDxCommon->CreateBufferResource(
		mDxCommon->device_.Get(), sizeof(VertexDataForGrid) * 24);

	// リソースの先頭のアドレスから使う
	mVertexBufferView.BufferLocation = mVertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点分のサイズ
	mVertexBufferView.SizeInBytes = UINT(sizeof(VertexDataForGrid) * 24);
	// 1頂点あたりのサイズ
	mVertexBufferView.StrideInBytes = sizeof(VertexDataForGrid);

	// 頂点リソースにデータを書き込む
	mVertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mVertexData));// 書き込むためのアドレスを取得

	// 右面 描画インデックスは[0,1,2][2,1,3]で内側を向く
	mVertexData[0].position = { 1.0f,1.0f,1.0f,1.0f };
	mVertexData[1].position = { 1.0f,1.0f,-1.0f,1.0f };
	mVertexData[2].position = { 1.0f,-1.0f,1.0f,1.0f };
	mVertexData[3].position = { 1.0f,-1.0f,-1.0f,1.0f };
	// 左面 描画インデックスは[4,5,6][6,5,7]
	mVertexData[4].position = { -1.0f,1.0f,-1.0f,1.0f };
	mVertexData[5].position = { -1.0f,1.0f,1.0f,1.0f };
	mVertexData[6].position = { -1.0f,-1.0f,-1.0f,1.0f };
	mVertexData[7].position = { -1.0f,-1.0f,1.0f,1.0f };

	// 前面 描画インデックスは[8,9,10][10,9,11]
	mVertexData[8].position = { -1.0f,1.0f,1.0f,1.0f };
	mVertexData[9].position = { 1.0f,1.0f,1.0f,1.0f };
	mVertexData[10].position = { -1.0f,-1.0f,1.0f,1.0f };
	mVertexData[11].position = { 1.0f,-1.0f,1.0f,1.0f };
	// 後面 描画インデックスは[12,13,14][14,13,15]
	mVertexData[12].position = { -1.0f,1.0f,-1.0f,1.0f };
	mVertexData[13].position = { 1.0f,1.0f,-1.0f,1.0f };
	mVertexData[14].position = { -1.0f,-1.0f,-1.0f,1.0f };
	mVertexData[15].position = { 1.0f,-1.0f,-1.0f,1.0f };
	// 上面 描画インデックスは[16,17,18][18,17,19]
	mVertexData[16].position = { -1.0f,1.0f,-1.0f,1.0f };
	mVertexData[17].position = { 1.0f,1.0f,-1.0f,1.0f };
	mVertexData[18].position = { -1.0f,1.0f,1.0f,1.0f };
	mVertexData[19].position = { 1.0f,1.0f,1.0f,1.0f };
	// 下面 描画インデックスは[20,21,22][22,21,23]
	mVertexData[20].position = { -1.0f,-1.0f,-1.0f,1.0f };
	mVertexData[21].position = { 1.0f,-1.0f,-1.0f,1.0f };
	mVertexData[22].position = { -1.0f,-1.0f,1.0f,1.0f };
	mVertexData[23].position = { 1.0f,-1.0f,1.0f,1.0f };

	// カラーはまとめて設定する
	for (int32_t i = 0; i < 24; i++) {
		mVertexData[i].color = { 0.8f,0.8f,0.8f,1.0f };
	}

}

void AABBCollider::CreateIndex(){

	mIndexResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(uint32_t) * 36);
	mIndexBufferView.BufferLocation = mIndexResource->GetGPUVirtualAddress();
	mIndexBufferView.SizeInBytes = sizeof(uint32_t) * 36;
	mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;


	// インデックスリソースにデータを書き込む
	uint32_t* indexData = nullptr;
	mIndexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
	indexData[3] = 2; indexData[4] = 1; indexData[5] = 3;

	indexData[6] = 4; indexData[7] = 5; indexData[8] = 6;
	indexData[9] = 6; indexData[10] = 5; indexData[11] = 7;

	indexData[12] = 8; indexData[13] = 9; indexData[14] = 10;
	indexData[15] = 10; indexData[16] = 9; indexData[17] = 11;

	indexData[18] = 12; indexData[19] = 13; indexData[20] = 14;
	indexData[21] = 14;	indexData[22] = 13; indexData[23] = 15;

	indexData[24] = 16;	indexData[25] = 17; indexData[26] = 18;
	indexData[27] = 18; indexData[28] = 17; indexData[29] = 19;

	indexData[30] = 20; indexData[31] = 21; indexData[32] = 22;
	indexData[33] = 22; indexData[34] = 21; indexData[35] = 23;


}

void AABBCollider::CreateRootSigneture() {

	// RootParamenter作成
	D3D12_ROOT_PARAMETER rootParameters[1] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
	rootParameters[0].Descriptor.ShaderRegister = 0; // b0 レジスタにバインド
	rootParameters[0].Descriptor.RegisterSpace = 0;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ
	descriptionRootSignature.NumStaticSamplers = 0;
	descriptionRootSignature.pStaticSamplers = nullptr;
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob
	);

	if (FAILED(hr)) {
		WinAPI::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	// バイナリを元に
	hr = mDxCommon->device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
	assert(SUCCEEDED(hr));

}

void AABBCollider::CreatePSO() {

	CreateRootSigneture();

	// InputLayoutの設定を行う
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};

	// 座標
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	// 色
	inputElementDescs[1].SemanticName = "COLOR";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	// BlendStateの設定を行う(P.34)
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	// RasterizerStateの設定を行う(P.36)
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをcompileする(P.37)
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = WinAPI::CompileShader(L"Shaders/Collider/Collider3D.VS.hlsl",
		L"vs_6_0", mDxCommon->dxcUtils, mDxCommon->dxcCompiler, mDxCommon->includeHandler);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = WinAPI::CompileShader(L"Shaders/Collider/Collider3D.PS.hlsl",
		L"ps_6_0", mDxCommon->dxcUtils, mDxCommon->dxcCompiler, mDxCommon->includeHandler);
	assert(pixelShaderBlob != nullptr);

	// PSOを生成する(P.38)
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = mRootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ 線
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	// どのように色を打ち込むかの設定(気にしなくていい)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//  DepthStencilの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化
	depthStencilDesc.DepthEnable = true;
	// 書き込みを行う
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//  DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 実際に生成
	HRESULT hr;
	hr = mDxCommon->device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&mGraphicsPipelineState));
	assert(SUCCEEDED(hr));

}