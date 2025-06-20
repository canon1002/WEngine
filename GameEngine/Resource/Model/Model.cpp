#include "Model.h"
#include "GameEngine/Object/camera/MainCamera.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
#include "GameEngine/Resource/Texture/TextureManager.h"

Model::~Model() {
	//mVertexResource.Reset();
	mCameraResource.Reset();
}


void Model::Init(const std::string& directrypath, const std::string& filename){

	// キューブマップのテクスチャ
	mTextureHandleCubeMap = 0;

	// モデル読み込み
	mModelData = Resource::LoadMultiModelFile(directrypath, filename);

	// 頂点リソース 生成
	CreateVertexResource();
	// Indexリソース 生成
	CreateIndexResource();
	// マテリアルリソース 生成
	CreateMaterialResource();

}

void Model::Update()
{

}

void Model::Draw()
{
	// 各メッシュごとに描画
	for (size_t i = 0; i < mModelData.meshes.size(); ++i) {
		const auto& mesh = mModelData.meshes[i];

		// 頂点バッファビュー・インデックスバッファビューをセット
		DirectXCommon::GetInstance()->mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferViews[i]);
		DirectXCommon::GetInstance()->mCommandList->IASetIndexBuffer(&mIndexBufferViews[i]);
		DirectXCommon::GetInstance()->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// マテリアルのCBufferの場所を指定
		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(0, mMaterialResources[i]->GetGPUVirtualAddress());
		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(3, mDirectionalLightResource->GetGPUVirtualAddress());
		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(4, mCameraResource->GetGPUVirtualAddress());

		// テクスチャをセット
		DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(2, mTextureHandles[i]);
		// キューブマップのテクスチャをセット
		if (mTextureHandleCubeMap != 0) {
			DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(5, mTextureHandleCubeMap);
		}

		// インデックスを使用してドローコール
		DirectXCommon::GetInstance()->mCommandList->DrawIndexedInstanced(UINT(mesh.indices.size()), 1, 0, 0, 0);
	}

}

void Model::DrawGUI(const std::string& label){
	label;
#ifdef _DEBUG
	//if (ImGui::TreeNode(label.c_str())) {
	//	// マテリアル
	//	if (ImGui::TreeNode("マテリアル")) {
	//		ImGui::DragFloat4("Color", &mMaterialData->color.r, 0.01f, 0.0f, 1.0f);
	//		ImGui::TreePop();// ノードを閉じる(この場合は "マテリアル" を閉じる)
	//	}
	//	if (ImGui::TreeNode("平行光源")) {
	//		ImGui::Checkbox("Lighting Flag", &mIsLighting);
	//		// Lightingの設定を変更できるように
	//		mMaterialData->enableLighting = mIsLighting;
	//		ImGui::DragFloat("Shininess", &mMaterialData->shininess, 0.05f, 0.0f, 1.0f);
	//		ImGui::DragFloat4("Color", &mDirectionalLightData->color.r);
	//		ImGui::DragFloat3("Directon", &mDirectionalLightData->direction.x, 0.1f, 0.0f, 1.0f);
	//		ImGui::DragFloat("Intensity", &mDirectionalLightData->intensity, 0.1f, 0.0f, 1.0f);
	//		ImGui::TreePop();
	//	}
	//	if (ImGui::TreeNode("環境マップ")) {
	//		ImGui::DragFloat("EnvironmentCoefficient", &mMaterialData->environmentCoefficient, 0.01f, 0.0f, 1.0f);
	//		ImGui::TreePop();// ノードを閉じる(この場合は "マテリアル" を閉じる)
	//	}

	//	ImGui::TreePop();
	//}

#endif // _DEBUG
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

void Model::CreateMaterialResource(){

	// メッシュごとにマテリアルリソースを生成
	for (const auto& mesh : mModelData.meshes) {

		// マテリアル用のResourceを作る
		auto resource = DirectXCommon::GetInstance()->
			CreateBufferResource(DirectXCommon::GetInstance()->
				mDevice.Get(), sizeof(Material));

		// マテリアルにデータを書き込む
		Material* materialData = nullptr;
		// 書き込むためのアドレスを取得
		resource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));


		// テクスチャの情報を転送
		int32_t textureHandle = 0;
		if (mesh.material.textureFilePath.empty()) {
			textureHandle = TextureManager::GetInstance()->mDefaultTextureIndex;
		}
		else {
			textureHandle = TextureManager::GetInstance()->LoadTexture(mesh.material.textureFilePath);
		}


		// 色の書き込み・Lightingの無効化
		materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
		materialData->enableLighting = true;
		materialData->shininess = 100.0f;
		materialData->uvTransform = MakeIdentity();
		materialData->environmentCoefficient = 1.0f;

		resource->Unmap(0, nullptr);

		// 配列に追加
		mMaterialResources.push_back(resource);
		mMaterialDatas.push_back(materialData); // ※必要ならUnmap前にコピー
		mTextureHandles.push_back(textureHandle);
	}


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
