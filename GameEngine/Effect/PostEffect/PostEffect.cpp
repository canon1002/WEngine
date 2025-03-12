#include "PostEffect.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Resource/Texture/TextureManager.h"

PostEffect::PostEffect() {}

PostEffect::~PostEffect(){

	// インスタンスを解放
	delete instance;
	instance = nullptr;
		
}

PostEffect* PostEffect::instance = nullptr;

PostEffect* PostEffect::GetInstance()
{
	if (instance == nullptr) {
		instance = new PostEffect ();
	}
	return instance;
}

void PostEffect::Finalize(){}

void PostEffect::Init() {

	// CopyImage用のPSO及びRootSignatureを生成する
	CreateGraphicsPipeline();

	CreateVertexResource();
	CreateTransformation();

	// PostEffect用のResourceを作る
	mPostEffects = nullptr;
	mPostEffectResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(PostEffects));
	// 書き込むためのアドレスを取得
	mPostEffectResource->Map(0, nullptr, reinterpret_cast<void**>(&mPostEffects));
	
	// スクリーン
	mPostEffects->screen.enableGrayScele = 0;
	mPostEffects->screen.enableScreenColor = 0;
	mPostEffects->screen.screenColor = { 1.0f,1.0f,1.0f,1.0f };
	// ビネット
	mPostEffects->vignette.enable = 1;
	mPostEffects->vignette.index = 0.1f;
	mPostEffects->vignette.multipliier = 0.2f;
	mPostEffects->vignette.color = { 0.0f,0.0f,0.0f,0.0f };
	// 赤色ビネット
	mPostEffects->redVignnete.enable = 0;
	mPostEffects->redVignnete.index = 0.0f;
	mPostEffects->redVignnete.multipliier = 2.8f;
	mPostEffects->redVignnete.color = { 0.0f,0.4f,0.4f,1.0f };
	// ぼかし
	mPostEffects->smooting.useBox = 0;
	mPostEffects->smooting.useGaussian = 0;
	mPostEffects->smooting.kernelSize = 3;
	mPostEffects->smooting.gaussianSigma = 2.0f;
	// アウトライン
	mPostEffects->outline.luminance.enable = 0;
	mPostEffects->outline.luminance.multipliier = 6.0f;
	mPostEffects->outline.depth.enable = 0;
	mPostEffects->outline.depth.projectionInverse = Inverse(MainCamera::GetInstance()->GetProjectionMatrix());
	

	// 画像を設定(エラー回避)
	//mTextureHandle = TextureManager::GetInstance()->LoadTexture("uvChecker.png");
	mTextureHandle = DirectXCommon::GetInstance()->mSrv->CreateRenderTextureSRV(DirectXCommon::GetInstance()->mRtv->mRenderTextureResource.Get());
	mDepthStencilHandle = DirectXCommon::GetInstance()->mSrv->CreateDepthSRV(DirectXCommon::GetInstance()->mDsv->mDepthStencilTextureResource.Get());

}

void PostEffect::Update() {

	// 逆行列を取得
	mPostEffects->outline.depth.projectionInverse = Inverse(MainCamera::GetInstance()->GetProjectionMatrix());
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

void PostEffect::PreDraw(){
	// RootSignatureを設定。PSOに設定しているが、別途設定が必要
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootSignature(rootSignature.Get());
	DirectXCommon::GetInstance()->mCommandList->SetPipelineState(graphicsPipelineState.Get());
}

void PostEffect::Draw() {

	DirectXCommon::GetInstance()->mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	DirectXCommon::GetInstance()->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルのCBufferの場所を指定
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(0, mPostEffectResource->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を指定
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(1, mWvpResource->GetGPUVirtualAddress());
	// テクスチャをセット
	DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(2, mTextureHandle);
	// DepthTextureを設定
	DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(3, mDepthStencilHandle);

	// インスタンス生成
	DirectXCommon::GetInstance()->mCommandList->DrawInstanced(3, 1, 0, 0);

}

void PostEffect::Debug()
{

#ifdef _DEBUG


	// スクリーン
	if (ImGui::BeginMenu("Screen")) {
		ImGui::SliderInt("GaryScale Enable", &mPostEffects->screen.enableGrayScele, 0, 1);
		ImGui::SliderInt("ScrreenColor Enable", &mPostEffects->screen.enableScreenColor, 0, 1);
		ImGui::ColorEdit4("SctreenColor", &mPostEffects->screen.screenColor.x);
		ImGui::EndMenu();
	}

	// ビネット
	if (ImGui::BeginMenu("Viggnete")) {
		ImGui::SliderInt("Enable", &mPostEffects->vignette.enable, 0, 1);
		ImGui::DragFloat("Multipliier", &mPostEffects->vignette.multipliier, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("Index", &mPostEffects->vignette.index, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat4("Color", &mPostEffects->vignette.color.x, 0.01f, 0.0f, 1.0f);
		ImGui::EndMenu();
	}

	// 赤色ビネット
	if (ImGui::BeginMenu("RedViggnete")) {
		ImGui::SliderInt("Enable", &mPostEffects->redVignnete.enable, 0, 1);
		ImGui::DragFloat("Multipliier", &mPostEffects->redVignnete.multipliier, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("Index", &mPostEffects->redVignnete.index, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat4("Color", &mPostEffects->redVignnete.color.x, 0.01f, 0.0f, 1.0f);
		ImGui::EndMenu();
	}

	// ぼかし
	if (ImGui::BeginMenu("Smooting")) {
		ImGui::SliderInt("Use BoxFilter", &mPostEffects->smooting.useBox, 0, 1);
		ImGui::SliderInt("Use GuaseFilter", &mPostEffects->smooting.useGaussian, 0, 1);
		ImGui::SliderInt("KernelSize", &mPostEffects->smooting.kernelSize, 1, 9);
		ImGui::DragFloat("GaussianSigma", &mPostEffects->smooting.gaussianSigma, 0.01f, 0.0f, 10.0f);
		ImGui::EndMenu();
	}

	// アウトライン
	if (ImGui::BeginMenu("Outline")) {
		ImGui::SliderInt("Luminance Enable", &mPostEffects->outline.luminance.enable, 0, 1);
		ImGui::DragFloat("Multipliier", &mPostEffects->outline.luminance.multipliier, 0.1f, 0.0f, 60.0f);
		ImGui::SliderInt("Depth Enable", &mPostEffects->outline.depth.enable, 0, 1);
		ImGui::EndMenu();
	}

#endif // _DEBUG

}

void PostEffect::CreateRootSignature(){
	
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
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
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

	// DepthStencilResource用
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTable
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRangeDepth;
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeDepth);// 

	// RootSignatureを生成する
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	// Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[2] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイナリフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;	// ありったけのMipMapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う

	// PointFillter用
	staticSamplers[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;// ポイントフィルタ
	staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[1].MaxLOD = D3D12_FLOAT32_MAX;	// ありったけのMipMapを使う
	staticSamplers[1].ShaderRegister = 1;// レジスタ番号0を使う
	staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う

	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob
	);

	if (FAILED(hr)) {
		WinApp::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	// バイナリを元に
	hr = DirectXCommon::GetInstance()->mDevice->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

}

void PostEffect::CreateGraphicsPipeline(){

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
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをcompileする(P.37)
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = WinApp::CompileShader(L"Shaders/PostEffect/CopyImage.VS.hlsl",
		L"vs_6_0", DirectXCommon::GetInstance()->dxcUtils, DirectXCommon::GetInstance()->dxcCompiler, DirectXCommon::GetInstance()->includeHandler);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = WinApp::CompileShader(L"Shaders/PostEffect/CopyImage.PS.hlsl",
		L"ps_6_0", DirectXCommon::GetInstance()->dxcUtils, DirectXCommon::GetInstance()->dxcCompiler, DirectXCommon::GetInstance()->includeHandler);
	assert(pixelShaderBlob != nullptr);


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
	hr = DirectXCommon::GetInstance()->mDevice->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));


}

//
void PostEffect::CreateVertexResource() {

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	mVertexResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(VertexData) * 3);
	// 頂点バッファビューを作成する
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

//
void PostEffect::CreateTransformation() {

	// Transformation用のResourceを作る
	mWvpResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(Matrix4x4));
	// データを書き込む
	// 書き込むためのアドレスを取得
	mWvpResource->Map(0, nullptr, reinterpret_cast<void**>(&mWvpData));
	// 単位行列を書き込む
	*mWvpData = MainCamera::GetInstance()->GetViewProjectionMatrix();

}