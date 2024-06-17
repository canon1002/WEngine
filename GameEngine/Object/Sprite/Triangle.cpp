#include "Triangle.h"
#include "GameEngine/Object/Camera/MainCamera.h"

Triangle::Triangle() {}

Triangle::~Triangle()
{
	delete mWvpData;
	delete vertexData;
	delete wvpData;
	delete mVertexData;
	delete materialData;
}

void Triangle::Initialize(DirectXCommon* dxCommon,CameraCommon* camera) {

	mDxCommon = dxCommon;
	mCamera = camera;


	CreateVertexResource();
	CreateTransformationRsource();
	CreateBufferView();

	mWvpResource->SetName(L"Tri");
	materialResource->SetName(L"Tri");
	mVertexResource->SetName(L"Tri");
	
}

void Triangle::Update() {

}

void Triangle::Draw() {
	
	// カメラのワールド行列
	cameraM = MakeAffineMatrix(Vector3{ 1.0f,1.0f,1.0f }, 
		Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,-5.0f });
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = Inverse(cameraM);
	// 正規化デバイス座標系(NDC)に変換(正射影行列をかける)
	pespectiveM = MakePerspectiveMatrix(0.45f, (1280.0f / 720.0f), 0.1f, 100.0f);
	// WVPにまとめる
	wvpM = Multiply(viewM,pespectiveM);
	// 三角形のワールド行列とWVP行列を掛け合わした行列を代入
	*mWvpData = Multiply(mWorldTransform.GetWorldMatrix(), wvpM);

	mDxCommon->commandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	mDxCommon->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// CBV設定

	// マテリアルのCBufferの場所を指定
	mDxCommon->commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を指定
	mDxCommon->commandList->SetGraphicsRootConstantBufferView(1, mWvpResource->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	mDxCommon->commandList->SetGraphicsRootDescriptorTable(2, mDxCommon->srv_->textureData_.at(1).textureSrvHandleGPU);

	// インスタンス生成
	mDxCommon->commandList->DrawInstanced(3, 1, 0, 0);

}

//
void Triangle::CreateVertexResource() {

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	mVertexResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(VertexData) * 3);

	// マテリアル用のResourceを作る
	materialResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(VertexData));
	// マテリアルにデータを書き込む
	materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 色を書き込む
	*materialData = Color(1.0f, 1.0f, 1.0f, 1.0f);

}

//
void Triangle::CreateTransformationRsource() {

	// Transformation用のResourceを作る
	mWvpResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), sizeof(Matrix4x4));
	// データを書き込む
	// 書き込むためのアドレスを取得
	mWvpResource->Map(0, nullptr, reinterpret_cast<void**>(&mWvpData));
	// 単位行列を書き込む
	*mWvpData = mCamera->GetViewProjectionMatrix();

}

//
void Triangle::CreateBufferView() {

	// VertexBufferViewを作成する(P.43)
	// 頂点バッファビューを作成する
	mVertexBufferView = {};
	// リソースの先頭のアドレスから使う
	mVertexBufferView.BufferLocation = mVertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点3つ分のサイズ
	mVertexBufferView.SizeInBytes = sizeof(VertexData) * 3;
	// 1頂点あたりのサイズ
	mVertexBufferView.StrideInBytes = sizeof(VertexData);

	// Resourceにデータを書き込む

	// 書き込むためのアドレスを取得
	mVertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mVertexData));
	// 左下
	mVertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	mVertexData[0].texcoord = { 0.0f,1.0f };
	// 上
	mVertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	mVertexData[1].texcoord = { 0.5f,0.0f };
	// 右下
	mVertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	mVertexData[2].texcoord = { 1.0f,1.0f };

}

//
void Triangle::DrawBeginResource() {


}