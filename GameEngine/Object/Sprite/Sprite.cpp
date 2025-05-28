#include "Sprite.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
#include "GameEngine/Object/Camera/MainCamera.h"

void Sprite::Init() {

	mCamera = MainCamera::GetInstance();
	CreateVertexResource();
	CreateIndexResource();
	CreateTransformation();
	CreateBufferView();

	// テクスチャサイズをイメージに合わせる
	AdjustTextureSize();

}

void Sprite::Update() {

	mWorldTransform.translation.z = 0.1f;

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
		mUVTransform.scale,
		Vector3{ 0.0f,0.0f,mUVTransform.rotation.z },
		mUVTransform.translation
	);
	// 変換したデータを代入する
	mMaterialData->uvTransform = uvTransformMatrix;

	// アンカーポイント反映
	float left = (0.0f - mAnchorPoint.x) * mSpriteSize.x;
	float right = (1.0f - mAnchorPoint.x) * mSpriteSize.x;
	float top = (0.0f - mAnchorPoint.y) * mSpriteSize.y;
	float bottom = (1.0f - mAnchorPoint.y) * mSpriteSize.y;

	mVertexData[0].position = { left,top,1.0f,1.0f };
	mVertexData[1].position = { right,top,1.0f,1.0f };
	mVertexData[2].position = { left,bottom,1.0f,1.0f };
	mVertexData[3].position = { right,bottom,1.0f,1.0f };

	

	// テクスチャ範囲指定
	mTextureSize = mTextureFullSize;
	float tex_left = mTextureLeftTop.x / mTextureFullSize.x;
	float tex_right = (mTextureLeftTop.x + mTextureSize.x) / mTextureFullSize.x;
	float tex_top = mTextureLeftTop.y / mTextureFullSize.y;
	float tex_bottom = (mTextureLeftTop.y + mTextureSize.y) / mTextureFullSize.y;

	// 頂点リソースにデータを書き込む
	mVertexData[0].texcoord = { tex_left,tex_top };
	mVertexData[1].texcoord = { tex_right,tex_top };
	mVertexData[2].texcoord = { tex_left,tex_bottom };
	mVertexData[3].texcoord = { tex_right,tex_bottom };

}

void Sprite::Draw() {

	DirectXCommon::GetInstance()->mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
	DirectXCommon::GetInstance()->mCommandList->IASetIndexBuffer(&mIndexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	DirectXCommon::GetInstance()->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// CBV設定

	// マテリアルのCBufferの場所を指定
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(0, mMaterialResourceSprite->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を指定
	DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(1, mWvpResource->GetGPUVirtualAddress());
	
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(2, mTextureHandle);

	// インデックスを使用してドローコール
	DirectXCommon::GetInstance()->mCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

}

void Sprite::DrawGui(){

#ifdef _DEBUG

	// ImGuiを描画する
	if (ImGui::CollapsingHeader("Sprite")) {
		
		// テクスチャのハンドルを表示
		ImGui::Text("Texture Handle: %d", mTextureHandle);
		// テクスチャのフルサイズを表示
		ImGui::Text("Texture Size: (%.2f, %.2f)", mTextureSize.x, mTextureSize.y);
		// スプライトのサイズを表示
		ImGui::DragFloat2("Sprite Size", &mSpriteSize.x, 0.1f, 0.0f, 10000.0f);
		// スケールの表示・変更
		ImGui::DragFloat3("Scale", &mWorldTransform.scale.x, 0.01f, 0.0f, 10000.0f);
		// 回転
		ImGui::DragFloat("Rotation", &mWorldTransform.rotation.z, 0.01f, -3.14f, 3.14f);
		// 座標の表示
		ImGui::DragFloat3("Position", &mWorldTransform.translation.x, 1.0f, -10000.0f, 10000.0f);
		// アンカーポイントの表示
		ImGui::DragFloat2("Anchor Point", &mAnchorPoint.x, 0.01f,-1.0f,1.0f);
		// UV座標の表示
		ImGui::DragFloat3("UV Transform", &mUVTransform.scale.x,0.1f,-100.0f,100.0f);
		// UV座標の回転
		ImGui::DragFloat("UV Rotation", &mUVTransform.rotation.z, 0.01f, -3.14f, 3.14f);
		// UV座標の平行移動
		ImGui::DragFloat2("UV Translation", &mUVTransform.translation.x, 0.01f, -100.0f, 100.0f);
		// 色の表示
		Color color = mMaterialData->color;
		ImGui::ColorEdit4("Color", &color.r);
		// 色の設定
		mMaterialData->color = color;
		// アルファ値の表示
		ImGui::DragFloat("Alpha", &mMaterialData->color.a, 0.01f, 0.0f, 1.0f);

	}

#endif // _DEBUG

}

// インデックスデータの生成
void Sprite::CreateIndexResource() {

}

//
void Sprite::CreateVertexResource() {

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	mVertexResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(VertexData2D) * 6);

	// マテリアル用のResourceを作る
	mMaterialResourceSprite = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->mDevice.Get(), sizeof(Material2D));
	// マテリアルにデータを書き込む
	mMaterialData = nullptr;
	// テクスチャの情報を転送
	if (mTextureHandle == 0) {
		mTextureHandle = TextureManager::GetInstance()->mDefaultTextureIndex;
	}
	// 書き込むためのアドレスを取得
	mMaterialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&mMaterialData));
	// 色の書き込み・Lightingの無効化
	mMaterialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	// UVTransformを設定
	mMaterialData->uvTransform = MakeIdentity();
	mUVTransform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

}

//
void Sprite::CreateTransformation() {

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
	mVertexData[0].position = { 0.0f,0.0f,0.1f,1.0f };
	mVertexData[0].texcoord = { 0.0f,0.0f };
	//　右上
	mVertexData[1].position = { 360.0f,0.0f,0.1f,1.0f };
	mVertexData[1].texcoord = { 1.0f,0.0f };
	// 左下
	mVertexData[2].position = { 0.0f,360.0f,0.1f,1.0f };
	mVertexData[2].texcoord = { 0.0f,1.0f };
	// 右下
	mVertexData[3].position = { 360.0f,360.0f,0.1f,1.0f };
	mVertexData[3].texcoord = { 1.0f,1.0f };

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

void Sprite::AdjustTextureSize()
{
	// テクスチャメタデータ取得
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(mTextureHandle);
	mTextureFullSize.x = static_cast<float>(metadata.width);
	mTextureFullSize.y = static_cast<float>(metadata.height);

	// 画像サイズをテクスチャサイズに合わせる
	mSpriteSize = mTextureFullSize;

}
