#include "RenderCopyImage.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

RenderCopyImage::RenderCopyImage() {}

RenderCopyImage::~RenderCopyImage()
{
	//delete mWvpData;
	//delete vertexData;
	//delete materialData;
}

void RenderCopyImage::Initialize(DirectXCommon* dxCommon, CameraCommon* camera) {

	mDxCommon = dxCommon;
	mCamera = camera;

	// CopyImage用のPSO及びRootSignatureを生成する
	CreateGraphicsPipeline();

	CreateVertexResource();
	CreateTransformationRsource();
	CreateBufferView();

	mTextureHandle = mDxCommon->srv_->LoadTexture("uvChecker.png");
	mTextureHandle = mDxCommon->srv_->CreateRenderTextureSRV(mDxCommon->rtv_->renderTextureResource.Get());
}

void RenderCopyImage::Update() {

	// 書き込むためのアドレスを取得
	//fullScreenResource->Map(0, nullptr, reinterpret_cast<void**>(&fullScreenData));

#ifdef _DEBUG

	ImGui::Begin("RenderTexture");

	//画面全体のカラー変更
	ImGui::Checkbox("ScreenColor Flag", &effectFlags.isEnableScreenColor);
	fullScreenData->enableScreenColor = effectFlags.isEnableScreenColor;
	if (effectFlags.isEnableScreenColor) {
		// 色を変更
		Color newColor = { fullScreenData->screenColor.x,fullScreenData->screenColor.y,
			fullScreenData->screenColor.z,fullScreenData->screenColor.w};
		ImGui::ColorEdit4("SctreenColor", &newColor.r);
		fullScreenData->screenColor = Vector4(newColor.r,newColor.g,newColor.b,newColor.a);
	}
	// ビネット
	ImGui::Checkbox("Viggetting Flag", &effectFlags.isEnableViggetting);
	fullScreenData->enableVignetting = effectFlags.isEnableViggetting;
	if (effectFlags.isEnableViggetting) {
		ImGui::DragFloat("Multipliier", &fullScreenData->vigneMultipliier);
		ImGui::DragFloat("Index", &fullScreenData->vigneIndex);
	}

	// ぼかし(BoxFilter/GuaseFilter)
	ImGui::Checkbox("Smooting Flag", &effectFlags.isEnableSmooting);
	fullScreenData->enableScreenColor = effectFlags.isEnableScreenColor;
	if (effectFlags.isEnableSmooting) {

		// GuaseFilter
		ImGui::Checkbox("GaussianFilter Flag", &effectFlags.isEnableGaussianFilter);
		fullScreenData->enableGaussianFilter = effectFlags.isEnableGaussianFilter;
		if (effectFlags.isEnableGaussianFilter) {
			// GuaseFilter実行時、BoxFilterは実行しないようにする
			effectFlags.isEnableBoxFilter = false;
			fullScreenData->enableBoxFilter = effectFlags.isEnableBoxFilter;

			ImGui::DragInt("KernelSize", &fullScreenData->kernelSize,1,1,21);
			ImGui::DragFloat("Sigma", &fullScreenData->GaussianSigma);

		}

		// BoxFilter
		ImGui::Checkbox("BoxFilter Flag", &effectFlags.isEnableBoxFilter);
		fullScreenData->enableBoxFilter = effectFlags.isEnableBoxFilter;
		if (effectFlags.isEnableBoxFilter) {
			// BoxFilter実行時、GuaseFilterは実行しないようにする
			effectFlags.isEnableGaussianFilter = false;
			fullScreenData->enableGaussianFilter = effectFlags.isEnableGaussianFilter;

			ImGui::DragInt("kernelSize", &fullScreenData->kernelSize,1,1,21);
		}


	}
	ImGui::End();

#endif // _DEBUG


	// カメラのワールド行列
	cameraM = MakeAffineMatrix(Vector3{ 1.0f,1.0f,1.0f },
		Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,-5.0f });
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = Inverse(cameraM);
	// 正規化デバイス座標系(NDC)に変換(正射影行列をかける)
	pespectiveM = MakePerspectiveMatrix(0.45f, (1280.0f / 720.0f), 0.1f, 100.0f);
	// WVPにまとめる
	wvpM = Multiply(viewM, pespectiveM);
	// 三角形のワールド行列とWVP行列を掛け合わした行列を代入
	*mWvpData = Multiply(mWorldTransform.GetWorldMatrix(), wvpM);

}

void RenderCopyImage::PreDraw(){
	// RootSignatureを設定。PSOに設定しているが、別途設定が必要
	mDxCommon->commandList->SetGraphicsRootSignature(rootSignature.Get());
	mDxCommon->commandList->SetPipelineState(graphicsPipelineState.Get());
}

void RenderCopyImage::Draw() {

	
	mDxCommon->commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	mDxCommon->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// CBV設定

	// マテリアルのCBufferの場所を指定
	mDxCommon->commandList->SetGraphicsRootConstantBufferView(0, fullScreenResource->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を指定
	mDxCommon->commandList->SetGraphicsRootConstantBufferView(1, mWvpResource->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	mDxCommon->commandList->SetGraphicsRootDescriptorTable(2, mDxCommon->srv_->textureData_.at(mTextureHandle).textureSrvHandleGPU);

	// インスタンス生成
	mDxCommon->commandList->DrawInstanced(3, 1, 0, 0);

}

void RenderCopyImage::CreateRootSignature(){
	// RootSignatureを生成する
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// 複数枚のSRVを扱えるように一括で設定をする
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;// offset自動計算

	// RootParamenter作成
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う // b0のbと一致
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号とバインド // b0のと一致

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う // b0のbと一致
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号とバインド // b0のと一致

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	// Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイナリフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;	// ありったけのMipMapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);


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
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

}

void RenderCopyImage::CreateGraphicsPipeline(){

	// ルートシグネチャを生成する
	CreateRootSignature();

	// 頂点には何もデータ入力をしないので、InputLayoutは利用しない
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;

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
	// 裏面
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをcompileする(P.37)
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = WinAPI::CompileShader(L"Shaders/CopyImage.VS.hlsl",
		L"vs_6_0", mDxCommon->dxcUtils, mDxCommon->dxcCompiler, mDxCommon->includeHandler);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = WinAPI::CompileShader(L"Shaders/CopyImage.PS.hlsl",
		L"ps_6_0", mDxCommon->dxcUtils, mDxCommon->dxcCompiler, mDxCommon->includeHandler);
	assert(pixelShaderBlob != nullptr);

	// シェーダーリスト
	/*
	CopyImage	// 通常
	Grayscale	// グレースケール
	Vignette	// ビネット
	BoxFilter	// 平滑化(ぼかし)
	BoxFilter5x5
	BoxFilter9x9
	GaussianFilter // ガウスぼかし
	*/



	// PSOを生成する(P.38)
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
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
	// 利用するトポロジ(形状)のタイプ 三角
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように色を打ち込むかの設定(気にしなくていい)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//  DepthStencilの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	
	// Depthの機能を無効化
	// 全画面に対してなにか処理を施したいだけなので、比較も書き込みも必要ないのでDepth自体不要
	depthStencilDesc.DepthEnable = false;
	//  DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 実際に生成
	HRESULT hr;
	hr = mDxCommon->device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));


}

//
void RenderCopyImage::CreateVertexResource() {

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	vertexResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(VertexData) * 3);

	// PostEffect用のResourceを作る
	fullScreenData = nullptr;
	fullScreenResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(FullScereenEffect));
	// 書き込むためのアドレスを取得
	fullScreenResource->Map(0, nullptr, reinterpret_cast<void**>(&fullScreenData));

	// PostEffect用構造体にデータを書き込む
	fullScreenData->enableScreenColor = 0;
	fullScreenData->enableGrayScele = 0;
	fullScreenData->screenColor = { 1.0f,1.0f,1.0f,1.0f };
	fullScreenData->enableVignetting = 0;
	fullScreenData->vigneMultipliier = 0.8f;
	fullScreenData->vigneIndex = 0.8f;
	fullScreenData->enableSmooting = 0;
	fullScreenData->enableBoxFilter = 0;
	fullScreenData->enableGaussianFilter = 0;
	fullScreenData->kernelSize = 11;
	fullScreenData->GaussianSigma = 2.0f;
}

//
void RenderCopyImage::CreateTransformationRsource() {

	// Transformation用のResourceを作る
	mWvpResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(Matrix4x4));
	// データを書き込む
	// 書き込むためのアドレスを取得
	mWvpResource->Map(0, nullptr, reinterpret_cast<void**>(&mWvpData));
	// 単位行列を書き込む
	*mWvpData = mCamera->GetViewProjectionMatrix();

}

//
void RenderCopyImage::CreateBufferView() {

	// VertexBufferViewを作成する(P.43)
	// 頂点バッファビューを作成する
	vertexBufferView = {};
	// リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 3;
	// 1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// Resourceにデータを書き込む

	// 書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// 左下
	vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	// 上
	vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.5f,0.0f };
	// 右下
	vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };

}