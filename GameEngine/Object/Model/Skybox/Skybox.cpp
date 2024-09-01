#include "Skybox.h"
#include "GameEngine/Object/Model/ModelCommon.h"
#include "GameEngine/Object//Camera/MainCamera.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

Skybox* Skybox::instance = nullptr;

Skybox* Skybox::GetInstance() {
	if (instance == nullptr) {
		instance = new Skybox();
	}
	return instance;
}

Skybox::~Skybox() {
	mVertexResource.Reset();
	CameraResource.Reset();
}

void Skybox::Init(const std::string& directrypath, const std::string& filename) {

	mDxCommon = DirectXCommon::GetInstance();
	mCamera = MainCamera::GetInstance();
	mWorldTransform = new WorldTransform();
	mWorldTransform->scale = { 512.0f,512.0f,512.0f };
	mTextureHandle = mDxCommon->srv_->LoadTexture(directrypath + "/" + filename);
	CreateTransformation();
	CreateVertexResource();
	CreateMaterialResource();
	CreateGraphicsPipeline();
}

void Skybox::Update(){

	MainCamera* camera = MainCamera::GetInstance();
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = camera->GetViewMatrix();
	// 正規化デバイス座標系(NDC)に変換(正射影行列をかける)
	pespectiveM = camera->GetProjectionMatrix();
	// WVPにまとめる
	wvpM = camera->GetViewProjectionMatrix();
	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	mWvpData->WVP = Multiply(mWorldTransform->GetWorldMatrix(), wvpM);
	mWvpData->World = mWorldTransform->GetWorldMatrix();
}

void Skybox::Draw()
{
	//wvp用のCBufferの場所を指定
	mDxCommon->mCommandList->SetGraphicsRootConstantBufferView(1, mWvpResource->GetGPUVirtualAddress());


	mDxCommon->mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
	mDxCommon->mCommandList->IASetIndexBuffer(&indexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	mDxCommon->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// CBV設定

	// マテリアルのCBufferの場所を指定
	mDxCommon->mCommandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

	mDxCommon->mCommandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
	mDxCommon->mCommandList->SetGraphicsRootConstantBufferView(4, CameraResource->GetGPUVirtualAddress());

	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	mDxCommon->mCommandList->SetGraphicsRootDescriptorTable(2, mDxCommon->srv_->textureData_.at(mTextureHandle).textureSrvHandleGPU);

	// インデックスを使用してドローコール
	mDxCommon->mCommandList->DrawIndexedInstanced(36, 1, 0, 0,0);
}

void Skybox::DrawGUI(const std::string& label) {
	label;
#ifdef _DEBUG

	/// デバック情報を描画
	
	ImGui::Begin(label.c_str());
	if (ImGui::TreeNode("ワールドトランスフォーム")) {
		ImGui::SliderAngle("RotateX", &mWorldTransform->rotation.x);
		ImGui::SliderAngle("RotateY", &mWorldTransform->rotation.y);
		ImGui::SliderAngle("RotateZ", &mWorldTransform->rotation.z);
		ImGui::DragFloat3("Scale", &mWorldTransform->scale.x);
		ImGui::DragFloat3("Rotate", &mWorldTransform->rotation.x);
		ImGui::DragFloat3("translate", &mWorldTransform->translation.x);
		ImGui::TreePop();// ノードを閉じる(この場合は "ワールドトランスフォーム" を閉じる)
	}
	ImGui::BeginChild(label.c_str());
	// マテリアル
	if (ImGui::TreeNode("マテリアル")) {
		ImGui::DragFloat4("Color", &materialData_->color.a);
		ImGui::DragInt("EnableLighting", &materialData_->enableLighting, 1.0f, 0, 1);
		ImGui::DragFloat("Shininess", &materialData_->shininess);
		ImGui::TreePop();// ノードを閉じる(この場合は "マテリアル" を閉じる)
	}
	if (ImGui::TreeNode("平行光源")) {
		ImGui::DragFloat4("Color", &directionalLightDate->color.r);
		ImGui::DragFloat3("Directon", &directionalLightDate->direction.x, 0.1f, 0.0f, 1.0f);
		ImGui::DragFloat("Intensity", &directionalLightDate->intensity, 0.1f, 0.0f, 1.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("頂点データ")) {
		for (int32_t index = 0; index < 24; ++index) {
			std::string str = "頂点" + std::to_string(index);
			ImGui::DragFloat3(str.c_str(), &mVertexData[index].position.x);
		}
		ImGui::TreePop();
	}

	ImGui::EndChild();
	ImGui::End();

#endif // _DEBUG
}

void Skybox::PreDraw() {

	// RootSignatureを設定。PSOに設定しているが、別途設定が必要
	mDxCommon->mCommandList->SetGraphicsRootSignature(rootSignature.Get());
	mDxCommon->mCommandList->SetPipelineState(graphicsPipelineState.Get());

}

void Skybox::CreateTransformation() {

	// Transformation用のResourceを作る
	mWvpResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(TransformationMatrix));
	// データを書き込む
	// 書き込むためのアドレスを取得
	mWvpResource->Map(0, nullptr, reinterpret_cast<void**>(&mWvpData));
	// 単位行列を書き込む
	mWvpData->WVP = MakeAffineMatrix(Vector3{ 1.0f,1.0f,1.0f }, 
		Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f });
	mWvpData->World = MakeIdentity();
}

void Skybox::CreateVertexResource() {


	// 実際に頂点リソースを作る
	mVertexResource = mDxCommon->CreateBufferResource(
		mDxCommon->device_.Get(), sizeof(VertexData) * 24);

	// リソースの先頭のアドレスから使う
	mVertexBufferView.BufferLocation = mVertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点分のサイズ
	mVertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * 24);
	// 1頂点あたりのサイズ
	mVertexBufferView.StrideInBytes = sizeof(VertexData);

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

	indexResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(uint32_t) * 36);
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 36;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// インデックスリソースにデータを書き込む
	uint32_t* indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
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

void Skybox::CreateMaterialResource()
{
	// マテリアル用のResourceを作る
	materialResource = mDxCommon->
		CreateBufferResource(mDxCommon->
			device_.Get(), sizeof(Material));

	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 色の書き込み・Lightingの無効化
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = true;
	// UVTransformを設定
	materialData_->uvTransform = MakeIdentity();
	uvTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	materialData_->shininess = 100.0f;

	// Light
	directionalLightResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(DirectionalLight));
	// データを書き込む
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDate));
	directionalLightDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightDate->direction = { 0.0f,-1.0f,0.0f };
	directionalLightDate->intensity = 1.0f;

	// カメラデータ
	CameraResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(CameraForGPU));
	// データを書き込む
	CameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));
	mCamera->Initialize();
	cameraData->worldPosition = mCamera->GetTranslate();

}

void Skybox::CreateRootSignature() {

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
	D3D12_ROOT_PARAMETER rootParameters[5] = {};
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

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;	// レジスタ番号1を使う

	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 2;	// レジスタ番号2を使う

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

void Skybox::CreateGraphicsPipeline() {

	// ルートシグネチャを生成する
	CreateRootSignature();

	// InputLayoutの設定を行う(P.32)
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//inputElementDescs[2].SemanticName = "NORMAL";
	//inputElementDescs[2].SemanticIndex = 0;
	//inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

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
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = WinAPI::CompileShader(L"Shaders/Skybox.VS.hlsl",
		L"vs_6_0", mDxCommon->dxcUtils, mDxCommon->dxcCompiler, mDxCommon->includeHandler);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = WinAPI::CompileShader(L"Shaders/Skybox.PS.hlsl",
		L"ps_6_0", mDxCommon->dxcUtils, mDxCommon->dxcCompiler, mDxCommon->includeHandler);
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
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように色を打ち込むかの設定(気にしなくていい)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//  DepthStencilの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化
	depthStencilDesc.DepthEnable = true;
	// 全ピクセルがz=1となるため、書き込む必要がない
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	
	//  DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 実際に生成
	HRESULT hr;
	hr = mDxCommon->device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));

}
