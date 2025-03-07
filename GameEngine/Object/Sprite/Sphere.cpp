#include "Sphere.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Resource/Texture/TextureManager.h"

SphereEntity::SphereEntity() {}

SphereEntity::~SphereEntity()
{
	delete mWvpData;
	delete mVertexData;
	delete materialData;
	delete directionalLightDate;
}

void SphereEntity::Init() {

	CreateVertexResource();
	CreateTransformation();
	CreateBufferView();
}

void SphereEntity::Update() {

	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = Inverse(cameraM);
	// 正規化デバイス座標系(NDC)に変換(正射影行列をかける)
	pespectiveM = MakePerspectiveMatrix(0.45f, (1280.0f / 720.0f), 0.1f, 100.0f);
	// WVPにまとめる
	wvpM = Multiply(viewM, pespectiveM);
	// 三角形のワールド行列とWVP行列を掛け合わした行列を代入
	mWvpData->WVP = Multiply(mWorldTransform.GetWorldMatrix(), wvpM);
	mWvpData->World = mWorldTransform.GetWorldMatrix();
}

void SphereEntity::Draw() const {

	DirectXCommon::GetInstance()->mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	DirectXCommon::GetInstance()->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// CBV設定

	// マテリアルのCBufferの場所を指定
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を指定
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(1, mWvpResource->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->mDefaultTextureIndex);

	// インスタンス生成
	DirectXCommon::GetInstance()->mCommandList->DrawInstanced(((kSubdivision) * (kSubdivision) * 6), 1, 0, 0);

}

//
void SphereEntity::CreateVertexResource() {

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	mVertexResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(VertexData) * ((kSubdivision) * (kSubdivision) * 6));

	// マテリアル用のResourceを作る
	materialResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(Material));
	// マテリアルにデータを書き込む
	materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 色の書き込み・Lightingの有効化
	materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData->enableLighting = true;

	// Light
	directionalLightResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(DirectionalLight));
	// データを書き込む
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDate));
	directionalLightDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightDate->direction = { 0.0f,-1.0f,0.0f };
	directionalLightDate->intensity = 1.0f;
	

}

//
void SphereEntity::CreateTransformation() {

	// Transformation用のResourceを作る
	mWvpResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(TransformationMatrix));
	// データを書き込む
	// 書き込むためのアドレスを取得
	mWvpResource->Map(0, nullptr, reinterpret_cast<void**>(&mWvpData));
	// 単位行列を書き込む
	mWvpData->WVP = mCamera->GetViewProjectionMatrix();
	mWvpData->World = MakeIdentity();

}

//
void SphereEntity::CreateBufferView() {

	// VertexBufferViewを作成する(P.43)
	// 頂点バッファビューを作成する
	mVertexBufferView = {};
	// リソースの先頭のアドレスから使う
	mVertexBufferView.BufferLocation = mVertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点3つ分のサイズ
	mVertexBufferView.SizeInBytes = sizeof(VertexData) * ((kSubdivision) * (kSubdivision) * 6);
	// 1頂点あたりのサイズ
	mVertexBufferView.StrideInBytes = sizeof(VertexData);

	// Resourceにデータを書き込む

	// 書き込むためのアドレスを取得
	mVertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mVertexData));
	
	//　頂点位置の設定
	// 緯度の方向に分割　-π/2 ~ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex; //	現在の緯度を求める

		// 経度の方向に分割　0 ~ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			float lon = lonIndex * kLonEvery; //	現在の経度を求める

			Vector3 a = {
				cos(lat) * cos(lon) * rad,
				sin(lat) * rad,
				cos(lat) * sin(lon) * rad };
			Vector3 b = {
				cos(lat + (pi / kSubdivision)) * cos(lon) * rad,
				sin(lat + (pi / kSubdivision)) * rad,
				cos(lat + (pi / kSubdivision)) * sin(lon) * rad };
			Vector3 c = {
				cos(lat) * cos(lon + ((pi * 2) / kSubdivision)) * rad,
				sin(lat) * rad,
				cos(lat) * sin(lon + ((pi * 2) / kSubdivision)) * rad };
			Vector3 d = {
				cos(lat + (pi / kSubdivision)) * cos(lon + ((pi * 2) / kSubdivision)) * rad,
				sin(lat + (pi / kSubdivision)) * rad,
				cos(lat + (pi / kSubdivision)) * sin(lon + ((pi * 2) / kSubdivision)) * rad };

			// 頂点インデックスにデータを入力する

			// 左下
			mVertexData[start].position = { a.x,a.y,a.z,1.0f };
			mVertexData[start].texcoord.x = float(lonIndex) / float(kSubdivision);
			mVertexData[start].texcoord.y = 1.0f - float(latIndex-1) / float(kSubdivision) ;
			mVertexData[start].normal.x = mVertexData[start].position.x;
			mVertexData[start].normal.y = mVertexData[start].position.y;
			mVertexData[start].normal.z = mVertexData[start].position.z;
			//　左上
			mVertexData[start + 1].position = { b.x,b.y,b.z,1.0f };
			mVertexData[start + 1].texcoord.x = float(lonIndex) / float(kSubdivision);
			mVertexData[start + 1].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
			mVertexData[start + 1].normal.x = mVertexData[start + 1].position.x;
			mVertexData[start + 1].normal.y = mVertexData[start + 1].position.y;
			mVertexData[start + 1].normal.z = mVertexData[start + 1].position.z;
			// 右下
			mVertexData[start + 2].position = { c.x,c.y,c.z,1.0f };
			mVertexData[start + 2].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
			mVertexData[start + 2].texcoord.y = 1.0f - float(latIndex-1) / float(kSubdivision);
			mVertexData[start + 2].normal.x = mVertexData[start + 2].position.x;
			mVertexData[start + 2].normal.y = mVertexData[start + 2].position.y;
			mVertexData[start + 2].normal.z = mVertexData[start + 2].position.z;
			/// 2枚目
			// 左上
			mVertexData[start + 3].position = { b.x,b.y,b.z,1.0f };
			mVertexData[start + 3].texcoord.x = float(lonIndex) / float(kSubdivision);
			mVertexData[start + 3].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
			mVertexData[start + 3].normal.x = mVertexData[start + 3].position.x;
			mVertexData[start + 3].normal.y = mVertexData[start + 3].position.y;
			mVertexData[start + 3].normal.z = mVertexData[start + 3].position.z;
			//　右上
			mVertexData[start + 4].position = { d.x,d.y,d.z,1.0f };
			mVertexData[start + 4].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
			mVertexData[start + 4].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
			mVertexData[start + 4].normal.x = mVertexData[start + 4].position.x;
			mVertexData[start + 4].normal.y = mVertexData[start + 4].position.y;
			mVertexData[start + 4].normal.z = mVertexData[start + 4].position.z;
			// 右下
			mVertexData[start + 5].position = { c.x,c.y,c.z,1.0f };
			mVertexData[start + 5].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
			mVertexData[start + 5].texcoord.y = 1.0f-float(latIndex-1) / float(kSubdivision);
			mVertexData[start + 5].normal.x = mVertexData[start + 5].position.x;
			mVertexData[start + 5].normal.y = mVertexData[start + 5].position.y;
			mVertexData[start + 5].normal.z = mVertexData[start + 5].position.z;

		}
	}

}

//
void SphereEntity::DrawBeginResource() {


}