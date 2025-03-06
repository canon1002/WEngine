#include "EffectPixels.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/GameMaster/Framerate.h"
#include "GameEngine/Resource/Texture/TextureManager.h"


void EffectPixels::Init() {

	CreateVertexResource();
	CreateTransformation();
	CreateMaterial();

	// ワールド座標の初期化
	mWorldTransform = new WorldTransform();
	mWorldTransform->Init();
	mWorldTransform->scale = { 1.0f,1.0f,1.0f };

	// テクスチャの設定
	mTextureHandle = TextureManager::GetInstance()->LoadTexture("white2x2.dds");

	// 生存時間
	mActiveCountMax = 0.1f;
	mActiveCount = mActiveCountMax;
	

}

void EffectPixels::Update() {

	// Δtを定義 60fps固定してあるが、実時間を計測して可変fpsで動かせるようにしておきたい
	const float kDeltaTime = (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();

	// 生存時間をΔtの量だけ減少させる
	mActiveCount -= kDeltaTime;

	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	mWvpData->WVP = Multiply(mWorldTransform->GetWorldMatrix(), MainCamera::GetInstance()->GetViewProjectionMatrix());
	mWvpData->World = mWorldTransform->GetWorldMatrix();

}

void EffectPixels::Draw() {


	// Transfromをセット
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(1, mWvpResource->GetGPUVirtualAddress());
	// Vertexをセット
	DirectXCommon::GetInstance()->mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
	// Indexをセット
	DirectXCommon::GetInstance()->mCommandList->IASetIndexBuffer(&mIndexBufferView);
	// 形状を設定
	DirectXCommon::GetInstance()->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアル
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(0, mMaterialResource->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(3, mDirectionalLightResource->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(4, mCameraResource->GetGPUVirtualAddress());

	// テクスチャをセット
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->mTextureData.at(mTextureHandle).textureSrvHandleGPU);
	// CueMapのテクスチャをセット
	if (mTextureHandleCubeMap != 0) {
		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootDescriptorTable(5, TextureManager::GetInstance()->mTextureData.at(mTextureHandleCubeMap).textureSrvHandleGPU);
	}

	// インデックスを使用してドローコール
	DirectXCommon::GetInstance()->mCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void EffectPixels::DrawGui(std::string name)
{
#ifdef _DEBUG
	ImGui::Begin(name.c_str());
	ImGui::DragFloat3("TopB", &mVertexData[0].position.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("TopA", &mVertexData[1].position.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("BottomB", &mVertexData[2].position.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("BottomA", &mVertexData[3].position.x, 0.1f, -100.0f, 100.0f);
	ImGui::ColorEdit4("color", &mMaterialData->color.r);
	ImGui::DragFloat3("Scale", &mWorldTransform->scale.x, 0.05f, -10.0f, 10.0f);
	ImGui::DragFloat3("Rotate", &mWorldTransform->rotation.x, 0.01f, -6.28f, 6.28f);
	ImGui::DragFloat3("translate", &mWorldTransform->translation.x, 0.1f, -100.0f, 100.0f);
	ImGui::End();
#endif // _DEBUG
}

//
void EffectPixels::CreateVertexResource() {

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	mVertexResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(VertexData) * 4);
	

	// リソースの先頭のアドレスから使う
	mVertexBufferView.BufferLocation = mVertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点6つ分のサイズ
	mVertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点あたりのサイズ
	mVertexBufferView.StrideInBytes = sizeof(VertexData);
	// 書き込むためのアドレスを取得
	HRESULT hr=mVertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mVertexData));
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to map vertex buffer resource.");
	}

	/// 1枚目
	//　左上
	mVertexData[0].position = { 0.0f,1.0f,1.0f,1.0f };
	mVertexData[0].texcoord = { 0.0f,0.0f };
	mVertexData[0].normal = { 0.0f,0.0f,1.0f };
	//　右上
	mVertexData[1].position = { 1.0f,1.0f,1.0f,1.0f };
	mVertexData[1].texcoord = { 1.0f,0.0f };
	mVertexData[1].normal = { 0.0f,0.0f,1.0f };
	// 左下
	mVertexData[2].position = { 0.0f,0.0f,1.0f,1.0f };
	mVertexData[2].texcoord = { 0.0f,1.0f };
	mVertexData[2].normal = { 0.0f,0.0f,1.0f };
	// 右下
	mVertexData[3].position = { 1.0f,0.0f,1.0f,1.0f };
	mVertexData[3].texcoord = { 1.0f,1.0f };
	mVertexData[3].normal = { 0.0f,0.0f,1.0f };

	mIndexResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(uint32_t) * 6);
	mIndexBufferView.BufferLocation = mIndexResource->GetGPUVirtualAddress();
	mIndexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// インデックスリソースにデータを書き込む
	uint32_t* indexData = nullptr;
	mIndexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
	indexData[3] = 1; indexData[4] = 3; indexData[5] = 2;


}

//
void EffectPixels::CreateTransformation() {

	// Transformation用のResourceを作る
	mWvpResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(TransformationMatrix));
	// データを書き込む
	// 書き込むためのアドレスを取得
	mWvpResource->Map(0, nullptr, reinterpret_cast<void**>(&mWvpData));
	// 単位行列を書き込む
	mWvpData->WVP = MakeAffineMatrix(Vector3{ 1.0f,1.0f,1.0f },Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f });
	mWvpData->World = MakeIdentity();
}

void EffectPixels::CreateMaterial() {

	// マテリアル用のResourceを作る
	mMaterialResource = DirectXCommon::GetInstance()->
		CreateBufferResource(DirectXCommon::GetInstance()->
			mDevice.Get(), sizeof(Material));

	// マテリアルにデータを書き込む
	mMaterialData = nullptr;
	// 書き込むためのアドレスを取得
	mMaterialResource->Map(0, nullptr, reinterpret_cast<void**>(&mMaterialData));
	// 色の書き込み・Lightingの無効化
	mMaterialData->color = { 1.0f, 0.1f, 0.1f, 0.8f };
	mMaterialData->enableLighting = true;
	mMaterialData->shininess = 100.0f;
	// UVTransformを設定
	mMaterialData->uvTransform = MakeIdentity();
	mUVTransform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	

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

void EffectPixels::SetVertices(Vector3 beforeTop, Vector3 afterTop, Vector3 beforeButtom, Vector3 afterButtom)
{

	// 拡張点をセットする
	mVertexData[0].position = { beforeTop.x,beforeTop.y,beforeTop.z,1.0f };// 左上
	mVertexData[1].position = { afterTop.x,afterTop.y,afterTop.z,1.0f };// 右上
	mVertexData[2].position = { beforeButtom.x,beforeButtom.y,beforeButtom.z,1.0f };// 左下
	mVertexData[3].position = { afterButtom.x,afterButtom.y,afterButtom.z,1.0f };// 右下

}

bool EffectPixels::GetIsActive()const {

	// 生存時間が0.0f以下であればfalseを返す
	if (mActiveCount <= 0.0f) {
		return false;
	}

	return true;
}
