#include "Sprite.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

void Sprite::Initialize(DirectXCommon* dxCommon, CameraCommon* camera) {

	mDxCommon = dxCommon;
	mCamera = camera;
	CreateVertexResource();
	CreateIndexResource();
	CreateTransformationRsource();
	CreateBufferView();

	// テクスチャサイズをイメージに合わせる
	AdjustTextureSize();

}

void Sprite::Update() {

#ifdef _DEBUG

	/*ImGui::Begin("Sprite");
	ImGui::DragFloat3("Scale", &mWorldTransform.scale.x, 0.1f, 0.1f, 2.0f);
	ImGui::DragFloat3("Rotate", &mWorldTransform.rotate.x);
	ImGui::DragFloat3("Tranlate", &mWorldTransform.translate.x);
	ImGui::Spacing();
	ImGui::DragFloat2("UVScale", &uvTransform_.scale.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat2("UVTranlate", &uvTransform_.translate.x, 0.01f, -10.0f, 10.0f);
	ImGui::SliderAngle("UVRotate", &uvTransform_.rotate.z);
	ImGui::ColorEdit4("Color", &materialData->color.r);
	ImGui::End();*/

#endif // _DEBUG

	//mWorldTransform.rotate.z += 0.05f;

	// カメラのワールド行列
	cameraM = MakeAffineMatrix(Vector3(1.0f,1.0f,1.0f),Vector3(0.0f,0.0f,0.0f),Vector3(0.0f,0.0f,0.0f));
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = Inverse(cameraM);
	// 正規化デバイス座標系(NDC)に変換(正射影行列をかける)
	projectM = MakeOrthographicMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.1f, 100.0f);
	// WVPにまとめる
	wvpM = Multiply(viewM, projectM);
	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	mWvpData->WVP = Multiply(mWorldTransform.GetWorldMatrix(), wvpM);
	mWvpData->World = mWorldTransform.GetWorldMatrix();

	/// マテリアル・UVTransform
	Matrix4x4 uvTransformMatrix = MakeAffineMatrix(
		uvTransform_.scale,
		Vector3{ 0.0f,0.0f,uvTransform_.rotation.z },
		uvTransform_.translation
	);
	// 変換したデータを代入する
	materialData->uvTransform = uvTransformMatrix;

	// アンカーポイント反映
	float left = (0.0f - anchorPoint.x) * spriteSize.x;
	float right = (1.0f - anchorPoint.x) * spriteSize.x;
	float top = (0.0f - anchorPoint.y) * spriteSize.y;
	float bottom = (1.0f - anchorPoint.y) * spriteSize.y;

	mVertexData[0].position = { left,top,0.0f,1.0f };
	mVertexData[1].position = { right,top,0.0f,1.0f };
	mVertexData[2].position = { left,bottom,0.0f,1.0f };
	mVertexData[3].position = { right,bottom,0.0f,1.0f };

	

	// テクスチャ範囲指定
	textureSize_ = textureFullSize_;
	float tex_left = textureLeftTop_.x / textureFullSize_.x;
	float tex_right = (textureLeftTop_.x + textureSize_.x) / textureFullSize_.x;
	float tex_top = textureLeftTop_.y / textureFullSize_.y;
	float tex_bottom = (textureLeftTop_.y + textureSize_.y) / textureFullSize_.y;

	// 頂点リソースにデータを書き込む
	mVertexData[0].texcoord = { tex_left,tex_top };
	mVertexData[1].texcoord = { tex_right,tex_top };
	mVertexData[2].texcoord = { tex_left,tex_bottom };
	mVertexData[3].texcoord = { tex_right,tex_bottom };

}

void Sprite::Draw() {

	mDxCommon->commandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
	mDxCommon->commandList->IASetIndexBuffer(&indexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	mDxCommon->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// CBV設定

	// マテリアルのCBufferの場所を指定
	mDxCommon->commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を指定
	mDxCommon->commandList->SetGraphicsRootConstantBufferView(1, mWvpResource->GetGPUVirtualAddress());
	
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	mDxCommon->commandList->SetGraphicsRootDescriptorTable(2, mDxCommon->srv_->textureData_.at(mTextureHandle).textureSrvHandleGPU);

	// インデックスを使用してドローコール
	mDxCommon->commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

}

// インデックスデータの生成
void Sprite::CreateIndexResource() {

}

//
void Sprite::CreateVertexResource() {

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	mVertexResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(VertexData2D) * 6);

	// マテリアル用のResourceを作る
	materialResourceSprite = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(Material2D));
	// マテリアルにデータを書き込む
	materialData = nullptr;
	// テクスチャの情報を転送
	if (mTextureHandle == 0) {
		mTextureHandle = mDxCommon->srv_->defaultTexId_;
	}
	// 書き込むためのアドレスを取得
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 色の書き込み・Lightingの無効化
	materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	// UVTransformを設定
	materialData->uvTransform = MakeIdentity();
	uvTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

}

//
void Sprite::CreateTransformationRsource() {

	// Transformation用のResourceを作る
	mWvpResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(TransformationMatrix));
	// データを書き込む
	// 書き込むためのアドレスを取得
	mWvpResource->Map(0, nullptr, reinterpret_cast<void**>(&mWvpData));
	// 単位行列を書き込む
	mWvpData->WVP = mCamera->GetViewProjectionMatrix();
	mWvpData->World = MakeIdentity();
}

//
void Sprite::CreateBufferView() {

	// リソースの先頭のアドレスから使う
	mVertexBufferView.BufferLocation = mVertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点6つ分のサイズ
	mVertexBufferView.SizeInBytes = sizeof(VertexData2D) * 4;
	// 1頂点あたりのサイズ
	mVertexBufferView.StrideInBytes = sizeof(VertexData2D);

	// Resourceにデータを書き込む

	// 書き込むためのアドレスを取得
	mVertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mVertexData));
	
	/// 1枚目
	//　左上
	mVertexData[0].position = { 0.0f,0.0f,0.0f,1.0f };
	mVertexData[0].texcoord = { 0.0f,0.0f };
	//　右上
	mVertexData[1].position = { 360.0f,0.0f,0.0f,1.0f };
	mVertexData[1].texcoord = { 1.0f,0.0f };
	// 左下
	mVertexData[2].position = { 0.0f,360.0f,0.0f,1.0f };
	mVertexData[2].texcoord = { 0.0f,1.0f };
	// 右下
	mVertexData[3].position = { 360.0f,360.0f,0.0f,1.0f };
	mVertexData[3].texcoord = { 1.0f,1.0f };

	indexResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(uint32_t) * 6);
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// インデックスリソースにデータを書き込む
	uint32_t* indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
	indexData[3] = 1; indexData[4] = 3; indexData[5] = 2;
}

void Sprite::AdjustTextureSize()
{
	// テクスチャメタデータ取得
	const DirectX::TexMetadata& metadata = mDxCommon->srv_->GetMetaData(mTextureHandle);
	textureFullSize_.x = static_cast<float>(metadata.width);
	textureFullSize_.y = static_cast<float>(metadata.height);

	// 画像サイズをテクスチャサイズに合わせる
	spriteSize = textureFullSize_;

}
