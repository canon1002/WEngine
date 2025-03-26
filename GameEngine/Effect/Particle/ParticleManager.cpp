#include "ParticleManager.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/TextureManager.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/GameMaster/Framerate.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "App/BlackBoard.h"

ParticleManager* ParticleManager::instance = nullptr;

ParticleManager* ParticleManager::GetInstance(){
	
	if (instance == nullptr) {
		instance = new ParticleManager();
	}

	return instance;
}

ParticleManager::~ParticleManager() {

	// インスタンスを削除
	delete instance;
	instance = nullptr;

}

void ParticleManager::Init(){

	// ランダムエンジンの初期化
	mRandomEngine = std::mt19937(mSeedGenerator());
	// パイプランステート 生成
	CreatePipelineState();
	// 頂点リソースの生成
	CreateVertexResource();

}

void ParticleManager::Update(){

	// ビルボード行列の計算 (制作途中)
	Matrix4x4 billboardMat = MainCamera::GetInstance()->mWorldTransform->GetWorldMatrix();
	// カメラのワールド行列から平行移動成分を抜いたものを作成
	billboardMat.m[3][0] = 0.0f;
	billboardMat.m[3][1] = 0.0f;
	billboardMat.m[3][2] = 0.0f;

	// その他行列の取得・更新

	std::unordered_map<std::string, ParticleGroup>::iterator itGroup = mParticleGroups.begin();
	for (itGroup; itGroup != mParticleGroups.end(); itGroup++) {
		// イテレーターの位置を保持する数値
		itGroup->second.instanceCount = 0;

		// イテレーターを使用してfor文を回す
		for (std::list<Particle>::iterator it = itGroup->second.particleList.begin(); it != itGroup->second.particleList.end();) {

			// 生存期間の過ぎたパーティクルをリストから削除する
			if ((*it).lifeTime <= (*it).currentTime) {
				it = itGroup->second.particleList.erase(it);
				continue;
			}


			if (itGroup->second.instanceCount < kNumMaxInstance) {

				// パーティクルの移動を行う
				Vector3 vel = (*it).vel * BlackBoard::GetBattleFPS();
				(*it).worldTransform.translation -= vel;

				// 経過時間の加算
				(*it).currentTime += BlackBoard::GetBattleFPS();

				// 徐々に透明にする
				float alpha = 1.0f - ((*it).currentTime / (*it).lifeTime);

				// ワールド座標の計算
				Matrix4x4 worldMat = Multiply(Multiply((*it).worldTransform.GetScalingMatrix(), billboardMat),
					(*it).worldTransform.GetTranslationMatrix());
				itGroup->second.instancingData[itGroup->second.instanceCount].WVP = Multiply(
					worldMat, MainCamera::GetInstance()->GetViewProjectionMatrix());

				// インスタンスごとのワールド座標を更新
				itGroup->second.instancingData[itGroup->second.instanceCount].World = worldMat;

				// 透明度を更新
				(*it).color.a = alpha;

				// 色・透明度を更新
				itGroup->second.instancingData[itGroup->second.instanceCount].color = (*it).color;
				itGroup->second.instancingData[itGroup->second.instanceCount].color.a = alpha;

				#ifdef _DEBUG

				if (ImGui::TreeNode(std::to_string(itGroup->second.instanceCount).c_str())) {
					float treeScale = (*it).worldTransform.scale.x;
					ImGui::DragFloat("Scale", &treeScale, 0.05f);
					(*it).worldTransform.scale = { treeScale ,treeScale ,treeScale };
					ImGui::SliderAngle("RotateX", &(*it).worldTransform.rotation.x);
					ImGui::SliderAngle("RotateY", &(*it).worldTransform.rotation.y);
					ImGui::SliderAngle("RotateZ", &(*it).worldTransform.rotation.z);
					ImGui::DragFloat3("Tranlate", &(*it).worldTransform.translation.x);
					ImGui::DragFloat("lifeTime", &(*it).lifeTime);
					ImGui::Spacing();
					ImGui::DragFloat3("UVScale", &itGroup->second.uvTransform.scale.x, 0.01f, -10.0f, 10.0f);
					ImGui::DragFloat3("UVTranlate", &itGroup->second.uvTransform.translation.x, 0.01f, -10.0f, 10.0f);
					ImGui::SliderAngle("UVRotate", &itGroup->second.uvTransform.rotation.z);
					ImGui::ColorEdit4("Color", &(*it).color.r);
					ImGui::TreePop();
				}

				#endif // _DEBUG

				itGroup->second.instanceCount++;
			}

			// イテレーターを次に進める
			it++;
		}

	}

}

void ParticleManager::PreDraw(){

	// RootSignatureを設定
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
	// PSOを設定
	DirectXCommon::GetInstance()->mCommandList->SetPipelineState(mGraphicsPipelineState.Get());
	// 頂点バッファをセット
	DirectXCommon::GetInstance()->mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
	// 形状を三角形に設定
	DirectXCommon::GetInstance()->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void ParticleManager::Draw(){

	// グループごとに描画処理を行う
	std::unordered_map<std::string, ParticleGroup>::iterator itGroup = mParticleGroups.begin();
	for (itGroup; itGroup != mParticleGroups.end(); itGroup++) {
		
		// instancceCountttが1以上のときに描画処理を行う
		if (itGroup->second.instanceCount > 0) {

			// マテリアルのCBufferの場所を設定
			DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(
				0, itGroup->second.materialResource->GetGPUVirtualAddress());
			// インスタンシング番号をセット
			DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(
				1, itGroup->second.srvIndex);
			// テクスチャをセット
			DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(
				2, itGroup->second.material.srvIndex);
			// ドローコール
			DirectXCommon::GetInstance()->mCommandList->DrawInstanced(
				6, itGroup->second.instanceCount, 0, 0);
		}

	}

}

void ParticleManager::CreateParticleGroupe(const std::string name, const std::string textureFilePath){

	// 読み込み済みである場合なら早期リターン
	if (mParticleGroups.contains(name)) {
		return;
	}

	// SRVマネージャのポインタを取得
	SRV* srvManager = DirectXCommon::GetInstance()->mSrv.get();
	srvManager;
	// 上限チェック
	assert(srvManager->mUseIndex < srvManager->kMaxSRVCount);

	// 新規のパーティクルグループを追加
	mParticleGroups.insert(std::make_pair(name, ParticleGroup{}));

	// 新規のパーティクルグループを参照して書き込む
	ParticleGroup& particleGroup = mParticleGroups[name];
	// マテリアルデータにテクスチャファイルを記録
	particleGroup.material.filePath = textureFilePath;
	// マテリアルリソースの生成
	CreateMaterialResource(particleGroup);
	// インスタンシングリソースの生成
	CreateInstancingResource(particleGroup);
	// インスタンシング用にSRVを確保してSRVインデックスを記録
	particleGroup.srvIndex = SetInstancingBuffer(kNumMaxInstance, particleGroup.instancingResource);
	
}

void ParticleManager::Emit(const std::string& name, const Emitter& emitter){

	// パーティクルグループが存在しない場合はエラー
	assert(mParticleGroups.contains(name));

	for (uint32_t index = 0; index < emitter.count; index++) {
		// パーティクルを生成し、指定されたグループに登録する
		mParticleGroups[name].particleList.push_back(Create(emitter));
	}

}

Particle ParticleManager::Create(Emitter emitter){
	
	// 戻り値
	Particle particle{};
	// スケール
	particle.worldTransform.scale = Vector3(
		emitter.scaleDistr(mRandomEngine),
		emitter.scaleDistr(mRandomEngine),
		emitter.scaleDistr(mRandomEngine));
	// 回転量(今のところは固定)
	particle.worldTransform.rotation = { 0.0f,0.0f,0.0f };
	// 乱数で座標をずらす
	particle.worldTransform.translation = Vector3(
		emitter.pos.x + emitter.posXDistr(mRandomEngine),
		emitter.pos.y + emitter.posYDistr(mRandomEngine),
		emitter.pos.z + emitter.posZDistr(mRandomEngine)
		);
	// 移動量
	particle.vel = Vector3(
		emitter.velXDistr(mRandomEngine),
		emitter.velYDistr(mRandomEngine),
		emitter.velZDistr(mRandomEngine)
		);
	// 色・透明度
	float colorVal = emitter.colorDistr(mRandomEngine);
	particle.color = Color(
		colorVal,
		colorVal,
		colorVal,
		0.9f
		);
	// 生存時間の設定
	particle.lifeTime = emitter.timeDistr(mRandomEngine);
	particle.currentTime = 0;

	// 生成したパーティクルを返す
	return particle;
}

void ParticleManager::CreateRootSignature(){

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags =D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;// offset自動計算
	
	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;
	descriptorRangeForInstancing[0].NumDescriptors = 1;
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;// offset自動計算

	// RootParamenter作成
	D3D12_ROOT_PARAMETER rootParameters[3] = {};

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う // b0のbと一致
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号とバインド // b0のと一致

	// インスタンシング用
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing; // Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing); // Tableの中身の配列を指定


	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	rootSignatureDesc.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
	rootSignatureDesc.NumParameters = _countof(rootParameters); // 配列の長さ

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
	rootSignatureDesc.pStaticSamplers = staticSamplers;
	rootSignatureDesc.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,&signatureBlob,&errorBlob
	);

	if (FAILED(hr)) {
		WinApp::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	// バイナリを元に作成
	hr = DirectXCommon::GetInstance()->mDevice->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
	assert(SUCCEEDED(hr));

}

void ParticleManager::CreatePipelineState(){

	// RootSignatureの生成
	CreateRootSignature();

	// InputLayoutの設定を行う(P.32)
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "COLOR";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	// BlendStateの設定を行う(パーティクルはAdd Blendにする)
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
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
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = WinApp::CompileShader(L"Shaders/Particle.VS.hlsl",
		L"vs_6_0", DirectXCommon::GetInstance()->dxcUtils, DirectXCommon::GetInstance()->dxcCompiler, DirectXCommon::GetInstance()->includeHandler);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = WinApp::CompileShader(L"Shaders/Particle.PS.hlsl",
		L"ps_6_0", DirectXCommon::GetInstance()->dxcUtils, DirectXCommon::GetInstance()->dxcCompiler, DirectXCommon::GetInstance()->includeHandler);
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
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように色を打ち込むかの設定(気にしなくていい)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	
	//  DepthStencilの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化
	depthStencilDesc.DepthEnable = true;
	// 書き込みを行わないようにする
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//  DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 実際に生成
	HRESULT hr;
	hr = DirectXCommon::GetInstance()->mDevice->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&mGraphicsPipelineState));
	assert(SUCCEEDED(hr));

}

void ParticleManager::CreateVertexResource() {

	// 頂点データを作成
	mModelData.vertices.push_back(VertexData{ .position = {-1.0f,  1.0f,0.0f,1.0f},.texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	mModelData.vertices.push_back(VertexData{ .position = { 1.0f,  1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	mModelData.vertices.push_back(VertexData{ .position = {-1.0f, -1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
	mModelData.vertices.push_back(VertexData{ .position = {-1.0f, -1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
	mModelData.vertices.push_back(VertexData{ .position = { 1.0f,  1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	mModelData.vertices.push_back(VertexData{ .position = { 1.0f, -1.0f,0.0f,1.0f},.texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });

	// 頂点リソースを作成
	mVertexResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(VertexData) * mModelData.vertices.size());
	
	// リソースの先頭のアドレスから使う
	mVertexBufferView.BufferLocation = mVertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点分のサイズ
	mVertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * mModelData.vertices.size());
	// 1頂点あたりのサイズ
	mVertexBufferView.StrideInBytes = sizeof(VertexData);
	
	// 頂点リソースにデータを書き込む
	mVertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mVertexData));// 書き込むためのアドレスを取得
	std::memcpy(mVertexData, mModelData.vertices.data(), sizeof(VertexData) * mModelData.vertices.size());

}

void ParticleManager::CreateMaterialResource(ParticleGroup& particleGroup){

	// マテリアル用のResourceを作る
	particleGroup.materialResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(Material));
	// マテリアルにデータを書き込む
	particleGroup.material.materialData = nullptr;
	// 書き込むためのアドレスを取得
	particleGroup.materialResource->Map(0, nullptr,
		reinterpret_cast<void**>(&particleGroup.material.materialData));

	// テクスチャの情報を転送
	particleGroup.material.srvIndex = TextureManager::GetInstance()->LoadTexture(particleGroup.material.filePath);

	// 色の書き込み・Lightingの無効化
	particleGroup.material.materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	particleGroup.material.materialData->enableLighting = 0;
	particleGroup.material.materialData->shininess = 0.0f;
	// UVTransformを設定
	particleGroup.material.materialData->uvTransform = MakeIdentity();
	particleGroup.uvTransform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

}

void ParticleManager::CreateInstancingResource(ParticleGroup& particleGroup){

	// インスタンシング用のリソースを生成
	particleGroup.instancingResource = DirectXCommon::GetInstance()->CreateBufferResource(
		DirectXCommon::GetInstance()->mDevice.Get(), sizeof(ParticleForGPU) * kNumMaxInstance);

	// 書き込むためのアドレスを取得
	particleGroup.instancingResource->Map(
		0, nullptr, reinterpret_cast<void**>(&particleGroup.instancingData));

	for (int32_t index = 0; index < kNumMaxInstance; ++index) {
		// 単位行列を書き込む
		particleGroup.instancingData[index].WVP = MakeIdentity();
		particleGroup.instancingData[index].World = MakeIdentity();
		// カラーは透明にしておく
		particleGroup.instancingData[index].color = Color(1.0f, 1.0f, 1.0f, 0.0f);
	}

}

uint32_t ParticleManager::SetInstancingBuffer(int32_t kNumInstance, Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource) {
	
	// SRVマネージャのポインタを取得
	SRV* srvManager = DirectXCommon::GetInstance()->mSrv.get();
	// SRV上限チェック
	assert(srvManager->mUseIndex < srvManager->kMaxSRVCount);
	// 番号を取得
	uint32_t srvIndex= srvManager->Allocate();

	// インスタンシング用のSRVを設定
	srvManager->CreateSRVforStructuredBuffer(srvIndex, instancingResource.Get(), kNumInstance, sizeof(ParticleForGPU));
	return srvIndex;
}
