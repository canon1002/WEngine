#include "Grid3D.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

Grid3D::Grid3D(int32_t gridsize, float gridspace){
	// サイズと間隔を引数で取得
	mGridsize = gridsize;
	mGridspace = gridspace;
	// 初期化
	Init();
}

void Grid3D::Init(){

	mDxCommon = DirectXCommon::GetInstance();
	
	mWorldTransform = new WorldTransform();
	mWorldTransform->Init();
	CreateTransformation();
	CreateGraphicsPipeline();
	CreateIndexResource();
}

void Grid3D::Update()
{
#ifdef _DEBUG

	ImGui::Begin("Grid3D");
	ImGui::SliderAngle("RotateX", &mWorldTransform->rotation.x);
	ImGui::SliderAngle("RotateY", &mWorldTransform->rotation.y);
	ImGui::SliderAngle("RotateZ", &mWorldTransform->rotation.z);
	ImGui::DragFloat3("Scale", &mWorldTransform->scale.x, 0.05f, -10.0f, 10.0f);
	ImGui::DragFloat3("Rotate", &mWorldTransform->rotation.x, 0.01f, -6.28f, 6.28f);
	ImGui::DragFloat3("translate", &mWorldTransform->translation.x, 0.1f, -100.0f, 100.0f);
	if (ImGui::TreeNode("WVPData")) {
		if (ImGui::TreeNode("WVP")) {
			for (int i = 0; i < 4; ++i) {
				// Floatの4x4行列内の数値を表示
				ImGui::DragFloat4(("Row " + std::to_string(i)).c_str(), mWvpData->WVP.m[i]);
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("World")) {
			for (int i = 0; i < 4; ++i) {
				// Floatの4x4行列内の数値を表示
				ImGui::DragFloat4(("Row " + std::to_string(i)).c_str(), mWvpData->World.m[i]);
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG


	MainCamera* camera = MainCamera::GetInstance();
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = camera->GetViewMatrix();
	// WVPにまとめる
	wvpM = camera->GetViewProjectionMatrix();
	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	mWvpData->WVP = Multiply(mWorldTransform->GetWorldMatrix(), wvpM);
	mWvpData->World = mWorldTransform->GetWorldMatrix();
	
}

void Grid3D::PreDraw() {

	// RootSignatureを設定。PSOに設定しているが、別途設定が必要
	mDxCommon->mCommandList->SetGraphicsRootSignature(rootSignature.Get());
	mDxCommon->mCommandList->SetPipelineState(graphicsPipelineState.Get());

}

void Grid3D::Draw()
{
	//wvp用のCBufferの場所を指定
	mDxCommon->mCommandList->SetGraphicsRootConstantBufferView(0, mWvpResource->GetGPUVirtualAddress());
	
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	mDxCommon->mCommandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	// 配列を渡す(開始スロット番号、使用スロット数、VBV配列へのポインタ)
	mDxCommon->mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
	// IndexBufferViewをセット
	mDxCommon->mCommandList->IASetIndexBuffer(&indexBufferView);
	// インデックスを使用してドローコール
	mDxCommon->mCommandList->DrawIndexedInstanced(UINT(mGridIndices.size()), 1, 0, 0, 0);

}

void Grid3D::CreateTransformation(){

	// Transformation用のResourceを作る
	mWvpResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(TransformationMatrixForGrid3D));
	// データを書き込む
	// 書き込むためのアドレスを取得
	mWvpResource->Map(0, nullptr, reinterpret_cast<void**>(&mWvpData));
	// 単位行列を書き込む
	mWvpData->WVP = MakeAffineMatrix(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
	mWvpData->World = MakeIdentity();

}

void Grid3D::CreateVertexResource(){

}

void Grid3D::CreateIndexResource(){

    // グリッドの頂点とインデックスを作成
    for (int i = -mGridsize; i <= mGridsize; ++i) {
        for (int j = -mGridsize; j <= mGridsize; ++j) {
            for (int k = -mGridsize; k <= mGridsize; ++k) {
                // グリッドラインの頂点を追加
                // X軸
                mGridVertices.push_back({ 
                    Vector4(i * mGridspace, j * mGridspace, -mGridsize * mGridspace), 
                    Color(0.7f, 0.7f, 0.7f, 1.0f) });
                mGridVertices.push_back({
                    Vector4(i * mGridspace, j * mGridspace,  mGridsize * mGridspace), 
                    Color(0.7f, 0.7f, 0.7f, 1.0f) });
                // Y軸
                mGridVertices.push_back({ 
                    Vector4(i * mGridspace, -mGridsize * mGridspace, j * mGridspace),
                    Color(0.7f, 0.7f, 0.7f, 1.0f) });
                mGridVertices.push_back({ 
                    Vector4(i * mGridspace,  mGridsize * mGridspace, j * mGridspace),
                    Color(0.7f, 0.7f, 0.7f, 1.0f) });
                // Z軸
                mGridVertices.push_back({
                    Vector4(-mGridsize * mGridspace, i * mGridspace, j * mGridspace),
                    Color(0.7f, 0.7f, 0.7f, 1.0f) });
                mGridVertices.push_back({
                    Vector4( mGridsize * mGridspace, i * mGridspace, j * mGridspace),
                    Color(0.7f, 0.7f, 0.7f, 1.0f) });

                // インデックスを追加
                mGridIndices.push_back((UINT)mGridVertices.size() - 6);
                mGridIndices.push_back((UINT)mGridVertices.size() - 5);
                mGridIndices.push_back((UINT)mGridVertices.size() - 4);
                mGridIndices.push_back((UINT)mGridVertices.size() - 3);
                mGridIndices.push_back((UINT)mGridVertices.size() - 2);
                mGridIndices.push_back((UINT)mGridVertices.size() - 1);
            }
        }
    }

    // 実際に頂点リソースを作る
    mVertexResource = mDxCommon->CreateBufferResource(
        mDxCommon->device_.Get(), sizeof(VertexDataForGrid) * mGridVertices.size());

    // リソースの先頭のアドレスから使う
    mVertexBufferView.BufferLocation = mVertexResource->GetGPUVirtualAddress();
    // 使用するリソースサイズは頂点分のサイズ
    mVertexBufferView.SizeInBytes = UINT(sizeof(VertexDataForGrid) * mGridVertices.size());
    // 1頂点あたりのサイズ
    mVertexBufferView.StrideInBytes = sizeof(VertexDataForGrid);

    // 頂点リソースにデータを書き込む
    mVertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mVertexData));// 書き込むためのアドレスを取得
    std::memcpy(mVertexData, mGridVertices.data(), sizeof(VertexDataForGrid) * mGridVertices.size());


    // Indexは <uint32_t * Indexデータのサイズ> 分だけ確保する
    indexResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(uint32_t) * mGridIndices.size());
    // GPUアドレスを取得
    indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
    // Byte数は <uint32_t * Indexデータのサイズ>分
    indexBufferView.SizeInBytes = sizeof(uint32_t) * (uint32_t)mGridIndices.size();
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;
    // Rsourceに対してIndexの内容をコピーする
    uint32_t* mappedIndex = nullptr;
    indexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex));
    std::memcpy(mappedIndex, mGridIndices.data(), sizeof(uint32_t) * mGridIndices.size());


}

void Grid3D::CreateMaterialResource(){

}


void Grid3D::CreateRootSignature() {

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
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

}

void Grid3D::CreateGraphicsPipeline() {
	// ルートシグネチャを生成する
	CreateRootSignature();

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
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = WinAPI::CompileShader(L"Shaders/Grid3D.VS.hlsl",
		L"vs_6_0", mDxCommon->dxcUtils, mDxCommon->dxcCompiler, mDxCommon->includeHandler);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = WinAPI::CompileShader(L"Shaders/Grid3D.PS.hlsl",
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
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));


}
