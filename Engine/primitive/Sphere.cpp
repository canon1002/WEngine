#include "Sphere.h"
#include "../object/camera/MatrixCamera.h"

Sphere::Sphere() { this->Initialize(); }

Sphere::~Sphere()
{
	/*wvpResource->Release();
	materialResource->Release();
	vertexResource->Release();*/
}

void Sphere::Initialize() {

	dx_ = DirectXCommon::GetInstance();
	mainCamera_ = MatrixCamera::GetInstance();

	worldTransform_ = new WorldTransform;
	cameraWorldTransform_ = new WorldTransform;
	cameraWorldTransform_->translate.z = -5.0f;

	CreateVertexResource();
	CreateTransformationRsource();
	CreateBufferView();

}

void Sphere::Update() {


	ImGui::Begin("camera");
	ImGui::SliderFloat3("Scale", &cameraWorldTransform_->scale.x,0.0f,1.0f);
	ImGui::SliderFloat3("rotate", &cameraWorldTransform_->rotate.x, -2.0f, 2.0f);
	ImGui::SliderFloat3("translate", &cameraWorldTransform_->translate.x, -20.0f, 20.0f);
	ImGui::End();

	ImGui::Begin("Sphere");
	ImGui::DragFloat3("scale", &worldTransform_->scale.x, 0.01f, 0.0f, 5.0f);
	ImGui::DragFloat3("rotate", &worldTransform_->rotate.x, 0.01f, -2.0f, 2.0f);
	ImGui::DragFloat3("tranlate", &worldTransform_->translate.x, 0.01f, -2.0f, 2.0f);
	ImGui::Checkbox("useBallTex", &useBall);
	ImGui::End();

	ImGui::Begin("Light");
	ImGui::SliderFloat4("color", &directionalLightDate->color.x, 0.0f, 1.0f);
	ImGui::DragFloat3("directon", &directionalLightDate->direction.x, 0.05f, -10.0f, 10.0f);
	ImGui::SliderFloat("intensity", &directionalLightDate->intensity, 0.0f, 1.0f);
	ImGui::End();
	
	//　球体のワールド行列
	worldTransform_->worldM = W::Math::MakeAffineMatrix(
		worldTransform_->scale, worldTransform_->rotate, worldTransform_->translate);

	// カメラのワールド行列
	
	cameraM = W::Math::MakeAffineMatrix(
		cameraWorldTransform_->scale, cameraWorldTransform_->rotate, cameraWorldTransform_->translate);
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = W::Math::Inverse(cameraM);
	// 正規化デバイス座標系(NDC)に変換(正射影行列をかける)
	pespectiveM = W::Math::MakePerspectiveMatrix(0.45f, (1280.0f / 720.0f), 0.1f, 100.0f);
	// WVPにまとめる
	wvpM = W::Math::Multiply(viewM, pespectiveM);
	// 三角形のワールド行列とWVP行列を掛け合わした行列を代入
	wvpData->WVP = W::Math::Multiply(worldTransform_->worldM, wvpM);
	wvpData->World = worldTransform_->worldM;
}

void Sphere::Draw() const {

	dx_->commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	dx_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// CBV設定

	// マテリアルのCBufferの場所を指定
	dx_->commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を指定
	dx_->commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	dx_->commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	dx_->commandList->SetGraphicsRootDescriptorTable(2, dx_->srv_->textureData_.at(1).textureSrvHandleGPU);

	// インスタンス生成
	dx_->commandList->DrawInstanced(((kSubdivision) * (kSubdivision) * 6), 1, 0, 0);

}

//
void Sphere::CreateVertexResource() {

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	vertexResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(VertexData) * ((kSubdivision) * (kSubdivision) * 6));

	// マテリアル用のResourceを作る
	materialResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(Material));
	// マテリアルにデータを書き込む
	materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 色の書き込み・Lightingの有効化
	materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData->enableLighting = true;

	// Light
	directionalLightResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(DirectionalLight));
	// データを書き込む
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDate));
	directionalLightDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightDate->direction = { 0.0f,-1.0f,0.0f };
	directionalLightDate->intensity = 1.0f;
	

}

//
void Sphere::CreateTransformationRsource() {

	// Transformation用のResourceを作る
	wvpResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(TransformationMatrix));
	// データを書き込む
	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	wvpData->WVP = mainCamera_->GetWorldViewProjection();
	wvpData->World = W::Math::MakeIdentity();

}

//
void Sphere::CreateBufferView() {

	// VertexBufferViewを作成する(P.43)
	// 頂点バッファビューを作成する
	vertexBufferView = {};
	// リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * ((kSubdivision) * (kSubdivision) * 6);
	// 1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// Resourceにデータを書き込む

	// 書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	
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
			vertexData[start].position = { a.x,a.y,a.z,1.0f };
			vertexData[start].texcoord.x = float(lonIndex) / float(kSubdivision);
			vertexData[start].texcoord.y = 1.0f - float(latIndex-1) / float(kSubdivision) ;
			vertexData[start].normal.x = vertexData[start].position.x;
			vertexData[start].normal.y = vertexData[start].position.y;
			vertexData[start].normal.z = vertexData[start].position.z;
			//　左上
			vertexData[start + 1].position = { b.x,b.y,b.z,1.0f };
			vertexData[start + 1].texcoord.x = float(lonIndex) / float(kSubdivision);
			vertexData[start + 1].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
			vertexData[start + 1].normal.x = vertexData[start + 1].position.x;
			vertexData[start + 1].normal.y = vertexData[start + 1].position.y;
			vertexData[start + 1].normal.z = vertexData[start + 1].position.z;
			// 右下
			vertexData[start + 2].position = { c.x,c.y,c.z,1.0f };
			vertexData[start + 2].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
			vertexData[start + 2].texcoord.y = 1.0f - float(latIndex-1) / float(kSubdivision);
			vertexData[start + 2].normal.x = vertexData[start + 2].position.x;
			vertexData[start + 2].normal.y = vertexData[start + 2].position.y;
			vertexData[start + 2].normal.z = vertexData[start + 2].position.z;
			/// 2枚目
			// 左上
			vertexData[start + 3].position = { b.x,b.y,b.z,1.0f };
			vertexData[start + 3].texcoord.x = float(lonIndex) / float(kSubdivision);
			vertexData[start + 3].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
			vertexData[start + 3].normal.x = vertexData[start + 3].position.x;
			vertexData[start + 3].normal.y = vertexData[start + 3].position.y;
			vertexData[start + 3].normal.z = vertexData[start + 3].position.z;
			//　右上
			vertexData[start + 4].position = { d.x,d.y,d.z,1.0f };
			vertexData[start + 4].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
			vertexData[start + 4].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
			vertexData[start + 4].normal.x = vertexData[start + 4].position.x;
			vertexData[start + 4].normal.y = vertexData[start + 4].position.y;
			vertexData[start + 4].normal.z = vertexData[start + 4].position.z;
			// 右下
			vertexData[start + 5].position = { c.x,c.y,c.z,1.0f };
			vertexData[start + 5].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
			vertexData[start + 5].texcoord.y = 1.0f-float(latIndex-1) / float(kSubdivision);
			vertexData[start + 5].normal.x = vertexData[start + 5].position.x;
			vertexData[start + 5].normal.y = vertexData[start + 5].position.y;
			vertexData[start + 5].normal.z = vertexData[start + 5].position.z;

		}
	}

}

//
void Sphere::DrawBeginResource() {


}