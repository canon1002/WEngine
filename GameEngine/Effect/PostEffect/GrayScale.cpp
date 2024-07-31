#include "GrayScale.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

Grayscale::~Grayscale() {
	delete mEnableEffect;
}

void Grayscale::Init(){

	mDxCommon = DirectXCommon::GetInstance();
	mCamera = MainCamera::GetInstance();

	// グラフィックスパイプライン・ルートシグネチャ生成
	CreateGraphicsPipeline();
	// 頂点データ・リソースを生成
	CreateVertexResource();
	// エフェクトリソース生成
	CreateEffectResource();

	// レンダーターゲットの格納番号を受け取る
	textureHandle_ = mDxCommon->srv_->CreateRenderTextureSRV(mDxCommon->rtv_->mRenderTextureResource.Get());
}

void Grayscale::Update(){
}

void Grayscale::DrawGUI()
{
#ifdef _DEBUG
	if (ImGui::TreeNode("GrayScale")) {
		static bool enable = mEnableEffect;
		ImGui::Checkbox("Enable", &enable);
		if (enable == true) {
			*mEnableEffect = 1;
		}
		else {
			*mEnableEffect = 0;
		}
		ImGui::TreePop();
	}
#endif // _DEBUG
}

void Grayscale::PreDraw(){

	// リソースバリア



	// RootSignatureを設定。PSOに設定しているが、別途設定が必要
	mDxCommon->mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
	mDxCommon->mCommandList->SetPipelineState(mGraphicsPipelineState.Get());
}

void Grayscale::Draw(){

	// 頂点バッファビューをセット
	mDxCommon->mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	mDxCommon->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ポストエフェクトのパラメータのCBufferの場所を指定
	mDxCommon->mCommandList->SetGraphicsRootConstantBufferView(0, mEffectResource->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定
	mDxCommon->mCommandList->SetGraphicsRootDescriptorTable(1, mDxCommon->srv_->textureData_.at(textureHandle_).textureSrvHandleGPU);
	// インスタンス生成
	mDxCommon->mCommandList->DrawInstanced(3, 1, 0, 0);
}

void Grayscale::PostDraw(){

}


void Grayscale::CreateEffectResource() {

	// リソース生成
	mEffectResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(int32_t));
	// 書き込むためのアドレスを取得
	mEffectResource->Map(0, nullptr, reinterpret_cast<void**>(&mEnableEffect));
	// エフェクトを有効にしておく
	*mEnableEffect = 1;
}


void Grayscale::CreateGraphicsPipeline(){
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
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = WinAPI::CompileShader(L"Shaders/PostEffect/CopyImage.VS.hlsl",
		L"vs_6_0", mDxCommon->dxcUtils, mDxCommon->dxcCompiler, mDxCommon->includeHandler);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = WinAPI::CompileShader(L"Shaders/PostEffect/Grayscale.PS.hlsl",
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
	// 利用するトポロジ(形状)のタイプ 三角
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
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
	// Frag
	graphicsPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	// 実際に生成
	HRESULT hr;
	hr = mDxCommon->device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&mGraphicsPipelineState));
	assert(SUCCEEDED(hr));
}

void Grayscale::CreateRootSignature(){
	// 複数枚のSRVを扱えるように一括で設定をする
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;// offset自動計算

	D3D12_DESCRIPTOR_RANGE descriptorRangeDepth[1] = {};
	descriptorRangeDepth[0].BaseShaderRegister = 1;
	descriptorRangeDepth[0].NumDescriptors = 1;
	descriptorRangeDepth[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeDepth[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;// offset自動計算

	// RootParamenter作成
	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う // b0のbと一致
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号とバインド // b0のと一致

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	// RootSignatureを生成する
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
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
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
	assert(SUCCEEDED(hr));
}
