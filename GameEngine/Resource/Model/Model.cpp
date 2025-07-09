#include "Model.h"
#include "GameEngine/Object/camera/MainCamera.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
#include "GameEngine/Resource/Texture/TextureManager.h"
#include "GameEngine/Resource/Material/MaterialManager.h"

Model::~Model() {}


void Model::Init(const std::string& directrypath, const std::string& filename){

	// キューブマップのテクスチャ
	mTextureHandleCubeMap = 0;

	// モデル読み込み
	mModelData = Resource::LoadMultiModelFile(directrypath, filename);

	// 頂点リソース 生成
	CreateVertexResource();
	// Indexリソース 生成
	CreateIndexResource();

	// Light
	mDirectionalLightResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(DirectionalLight));
	// データを書き込む
	mDirectionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&mDirectionalLightData));
	mDirectionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	mDirectionalLightData->direction = { 0.0f,-1.0f,0.0f };
	mDirectionalLightData->intensity = 1.0f;

	// カメラデータ
	mCameraResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(CameraForGPU));
	// データを書き込む
	mCameraResource->Map(0, nullptr, reinterpret_cast<void**>(&mCameraData));
	mCameraData->worldPosition = MainCamera::GetInstance()->GetTranslate();

}

void Model::Update() {}

//
//void Model::Draw()
//{
//	// 各メッシュごとに描画
//	for (size_t i = 0; i < mModelData.meshes.size(); ++i) {
//		const auto& mesh = mModelData.meshes[i];
//
//		// 頂点バッファビュー・インデックスバッファビューをセット
//		DirectXCommon::GetInstance()->mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferViews[i]);
//		DirectXCommon::GetInstance()->mCommandList->IASetIndexBuffer(&mIndexBufferViews[i]);
//		DirectXCommon::GetInstance()->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(3, mDirectionalLightResource->GetGPUVirtualAddress());
//		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(4, mCameraResource->GetGPUVirtualAddress());
//
//		// テクスチャをセット
//		DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(2, mTextureHandles[i]);
//		// キューブマップのテクスチャをセット
//		if (mTextureHandleCubeMap != 0) {
//			DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(5, mTextureHandleCubeMap);
//		}
//
//		// インデックスを使用してドローコール
//		DirectXCommon::GetInstance()->mCommandList->DrawIndexedInstanced(UINT(mesh.indices.size()), 1, 0, 0, 0);
//	}
//
//}

//void Model::DrawSkinning(Skinning* skinning, const bool& isActive){
//
//	// スキニングが無効の場合、普通の3Dモデルを描画
//	if (isActive == false) {
//		Draw();
//		return;
//	}
//
//	// 各メッシュごとに描画
//	for (size_t i = 0; i < mModelData.meshes.size(); ++i) {
//		const auto& mesh = mModelData.meshes[i];
//
//		// 頂点バッファをセットする際に、スキニングの影響を受ける頂点データとインフルエンスデータをセットする
//		D3D12_VERTEX_BUFFER_VIEW vbvs[2]{};
//		vbvs[0] = mVertexBufferViews[i];// VertexDataのVBV
//		vbvs[1] = skinning->GetNowSkinCluster()->skinClusters[i].mInfluenceBufferView; // influenceのVBV
//
//		// 頂点バッファビュー・インデックスバッファビューをセット
//		DirectXCommon::GetInstance()->mCommandList->IASetVertexBuffers(0, 2, vbvs);
//		DirectXCommon::GetInstance()->mCommandList->IASetIndexBuffer(&mIndexBufferViews[i]);
//		DirectXCommon::GetInstance()->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//		// マテリアルのCBufferの場所を指定
//		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(0, mMaterialResources[i]->GetGPUVirtualAddress());
//		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(3, mDirectionalLightResource->GetGPUVirtualAddress());
//		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(4, mCameraResource->GetGPUVirtualAddress());
//
//		// スキニングのパレットをセット
//		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootDescriptorTable(6,
//			skinning->GetNowSkinCluster()->skinClusters[0].mPaletteSrvHandle.second);
//
//		// テクスチャをセット
//		DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(2, mTextureHandles[i]);
//		// キューブマップのテクスチャをセット
//		if (mTextureHandleCubeMap != 0) {
//			DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(5, mTextureHandleCubeMap);
//		}
//
//		// インデックスを使用してドローコール
//		DirectXCommon::GetInstance()->mCommandList->DrawIndexedInstanced(UINT(mesh.indices.size()), 1, 0, 0, 0);
//	}
//
//}

void Model::DrawGUI(const std::string& label){
	label;
#ifdef _DEBUG

	// モデル情報のGUI表示
	if (ImGui::CollapsingHeader(label.c_str())) {

		//// 各メッシュの情報を表示

		//// -- マテリアル -- //
		//if (ImGui::CollapsingHeader(std::string(label + "_Material").c_str())) {
		//	for (int32_t i = 0; i < mMaterialDatas.size(); i++) {

		//		// -- マテリアル 情報 -- //

		//		ImGui::PushID(i); // IDを設定して区別する
		//		ImGui::Text("Material %zu", i);
		//		ImGui::DragFloat4("Color", &mMaterialDatas[i]->color.r, 0.01f, 0.0f, 1.0f);
		//		ImGui::DragFloat("Shininess", &mMaterialDatas[i]->shininess, 0.05f, 0.0f, 1.0f);
		//		ImGui::DragFloat("Environment Coefficient", &mMaterialDatas[i]->environmentCoefficient, 0.01f, 0.0f, 1.0f);
		//		
		//		
		//		
		//		ImGui::PopID(); // IDを戻す
		//	}
		//}

		//// -- カメラ・光源 -- //

		//if (ImGui::CollapsingHeader("平行光源")) {
		//	ImGui::Checkbox("Lighting Flag", &mIsLighting);
		//	// Lightingの設定を変更できるように
		//	//mMaterialDatas[i]->enableLighting = mIsLighting;
		//	ImGui::DragFloat4("Color", &mDirectionalLightData->color.r);
		//	ImGui::DragFloat3("Directon", &mDirectionalLightData->direction.x, 0.1f, 0.0f, 1.0f);
		//	ImGui::DragFloat("Intensity", &mDirectionalLightData->intensity, 0.1f, 0.0f, 1.0f);
		//}
		//if (ImGui::CollapsingHeader("環境マップ")) {
		//	ImGui::DragFloat("EnvironmentCoefficient", &mMaterialDatas[i]->environmentCoefficient, 0.01f, 0.0f, 1.0f);
		//}
		//

	}

#endif // _DEBUG
}

void Model::Draw(const MaterialExt& materialExt){

	// 各メッシュごとに描画
	for (size_t i = 0; i < mModelData.meshes.size(); ++i) {
		const auto& mesh = mModelData.meshes[i];

		// 頂点バッファビュー・インデックスバッファビューをセット
		DirectXCommon::GetInstance()->mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferViews[i]);
		DirectXCommon::GetInstance()->mCommandList->IASetIndexBuffer(&mIndexBufferViews[i]);
		DirectXCommon::GetInstance()->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(0, materialExt.mMaterialResources[i]->GetGPUVirtualAddress());
		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(3, mDirectionalLightResource->GetGPUVirtualAddress());
		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(4, mCameraResource->GetGPUVirtualAddress());

		// テクスチャをセット
		DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(2, materialExt.mTextureHandles[i]);
		// キューブマップのテクスチャをセット
		if (mTextureHandleCubeMap != 0) {
			DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(5, mTextureHandleCubeMap);
		}

		// インデックスを使用してドローコール
		DirectXCommon::GetInstance()->mCommandList->DrawIndexedInstanced(UINT(mesh.indices.size()), 1, 0, 0, 0);
	}
}

void Model::DrawSkinning(const MaterialExt& materialExt, Skinning* skinning, const bool& isActive){

	// スキニングが無効の場合、普通の3Dモデルを描画
	if (isActive == false) {
		Draw(materialExt);
		return;
	}

	// 各メッシュごとに描画
	for (size_t i = 0; i < mModelData.meshes.size(); ++i) {
		const auto& mesh = mModelData.meshes[i];

		// 頂点バッファをセットする際に、スキニングの影響を受ける頂点データとインフルエンスデータをセットする
		D3D12_VERTEX_BUFFER_VIEW vbvs[2]{};
		vbvs[0] = mVertexBufferViews[i];// VertexDataのVBV
		vbvs[1] = skinning->GetNowSkinCluster()->skinClusters[i].mInfluenceBufferView; // influenceのVBV

		// 頂点バッファビュー・インデックスバッファビューをセット
		DirectXCommon::GetInstance()->mCommandList->IASetVertexBuffers(0, 2, vbvs);
		DirectXCommon::GetInstance()->mCommandList->IASetIndexBuffer(&mIndexBufferViews[i]);
		DirectXCommon::GetInstance()->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// マテリアルのCBufferの場所を指定
		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(0, materialExt.mMaterialResources[i]->GetGPUVirtualAddress());
		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(3, mDirectionalLightResource->GetGPUVirtualAddress());
		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(4, mCameraResource->GetGPUVirtualAddress());

		// スキニングのパレットをセット
		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootDescriptorTable(6,
			skinning->GetNowSkinCluster()->skinClusters[0].mPaletteSrvHandle.second);

		// テクスチャをセット
		DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(2, materialExt.mTextureHandles[i]);
		// キューブマップのテクスチャをセット
		if (mTextureHandleCubeMap != 0) {
			DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(5, mTextureHandleCubeMap);
		}

		// インデックスを使用してドローコール
		DirectXCommon::GetInstance()->mCommandList->DrawIndexedInstanced(UINT(mesh.indices.size()), 1, 0, 0, 0);
	}

}

//
void Model::CreateVertexResource() {

	// 頂点リソースを生成する

	// リソース・バッファのクリア
	mVertexResources.clear();
	mVertexBufferViews.clear();

	// メッシュごとに頂点リソースを生成
	for (const auto& mesh : mModelData.meshes) {

		// 頂点データのサイズを計算
		auto resource = DirectXCommon::GetInstance()->CreateBufferResource(
			DirectXCommon::GetInstance()->mDevice.Get(), sizeof(VertexData) * mesh.vertices.size());

		// 書き込むためのアドレスを取得
		void* mappedPtr = nullptr;
		resource->Map(0, nullptr, &mappedPtr);
		// 頂点リソースにデータを書き込む
		std::memcpy(mappedPtr, mesh.vertices.data(), sizeof(VertexData) * mesh.vertices.size());
		resource->Unmap(0, nullptr);

		// リソースをVertexResourcesに追加
		mVertexResources.push_back(resource);

		// VertexBufferViewを作成
		D3D12_VERTEX_BUFFER_VIEW view = {};
		// リソースの先頭のアドレスから使う
		view.BufferLocation = resource->GetGPUVirtualAddress();
		// 使用するリソースサイズは頂点分のサイズ
		view.SizeInBytes = UINT(sizeof(VertexData) * mesh.vertices.size());
		// 1頂点あたりのサイズ
		view.StrideInBytes = sizeof(VertexData);

		// VertexBufferViewをセット
		mVertexBufferViews.push_back(view);
	}

}

void Model::CreateIndexResource(){

	// メッシュごとにインデックスリソースを生成
	for (const auto& mesh : mModelData.meshes) {

		// Indexは <uint32_t * Indexデータのサイズ> 分だけ確保する
		auto resource = DirectXCommon::GetInstance()->CreateBufferResource(
			DirectXCommon::GetInstance()->mDevice.Get(), sizeof(uint32_t) * mesh.indices.size());

		// Rsourceに対してIndexの内容をコピーする
		uint32_t* mappedIndex = nullptr;
		resource->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex));
		std::memcpy(mappedIndex, mesh.indices.data(), sizeof(uint32_t) * mesh.indices.size());

		// リソースをVertexResourcesに追加
		mIndexResources.push_back(resource);

		// VertexBufferViewを作成
		D3D12_INDEX_BUFFER_VIEW view = {};
		// GPUアドレスを取得
		view.BufferLocation = resource->GetGPUVirtualAddress();
		// Byte数は <uint32_t * Indexデータのサイズ>分
		view.SizeInBytes = sizeof(uint32_t) * (uint32_t)mesh.indices.size();
		view.Format = DXGI_FORMAT_R32_UINT;

		// VertexBufferViewをセット
		mIndexBufferViews.push_back(view);

	}

}


std::unique_ptr<MaterialExt> Model::CreateMaterial() const
{
	// モデルの各メッシュからテクスチャファイルパスを取得
	std::vector<std::string> textureFilePaths;

	// 各メッシュのテクスチャファイルパスを取得
	for (const auto& mesh : mModelData.meshes) {
		// テクスチャファイルパスを取得
		textureFilePaths.push_back(mesh.textureFilePath);
	}

	// 各メッシュのテクスチャ情報を取得する
	std::unique_ptr<MaterialExt> material = std::make_unique<MaterialExt>();
	// メッシュのテクスチャ情報を渡して初期化
	material = MaterialExt::CreateMaterialResource(textureFilePaths);

	// 生成したマテリアルを返す
	return std::move(material);
}
