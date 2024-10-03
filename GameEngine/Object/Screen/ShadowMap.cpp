#include "ShadowMap.h"

using namespace DirectX;

void ShadowMap::Init()
{

	eyePos = XMVectorSet(0.0f, 0.0f, -5.0f, 1.0f);  // カメラの位置
	target = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);  // カメラの注視点
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);      // カメラの上方向
	viewMatrix = XMMatrixLookAtLH(eyePos, target, up);

	fovAngleY = XM_PI / 4.0f;  // 垂直方向の視野角（ラジアン）
	aspectRatio = 16.0f / 9.0f; // アスペクト比
	nearZ = 0.1f;  // 近クリップ面
	farZ = 1000.0f; // 遠クリップ面
	projectionMatrix = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);


	// ライト
	mLightPos = XMVectorSet(0.0f, 10.0f, -10.0f, 1.0f);
	mLightTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	mLightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	// ビュー行列
	mLightViewMtrix = XMMatrixLookAtLH(mLightPos, mLightTarget, mLightUp);
	
	// プロジェクション行列
	nearPlane = 1.0f;
	farPlane = 100.0f;
	fieldOfView = XM_PIDIV2;
	mLightProjectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, 1.0f, nearPlane, farPlane);

}

void ShadowMap::Upadate()
{
	// ビュープロジェクション行列の計算
	XMMATRIX lightViewProjectionMatrix = mLightViewMtrix * mLightProjectionMatrix;



}

void ShadowMap::Draw()
{
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 深度バッファのクリア
	dxCommon->mCommandList->OMSetRenderTargets(
		0,
		nullptr,
		FALSE,
		&dxCommon->dsv_->mDsvHandle);

	dxCommon->mCommandList->ClearDepthStencilView(
		dxCommon->dsv_->mDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

}

void ShadowMap::CreateScenePass()
{

	mScenePass;

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 定数バッファの作成
	mScenePassResource = dxCommon->CreateBufferResource(dxCommon->device_.Get(), sizeof(ScenePassConstants));

	// 頂点リソースにデータを書き込む
	mScenePassResource->Map(0, nullptr, reinterpret_cast<void**>(&mScenePass));// 書き込むためのアドレスを取得

	// 定数バッファへの書き込み
	// 関数を使ってXMMATRIXからFloat4x4に変換する
	XMStoreFloat4x4(&mScenePass->view,viewMatrix);
	XMStoreFloat4x4(&mScenePass->proj,projectionMatrix);
	XMStoreFloat4x4(&mScenePass->viewProj, viewMatrix * projectionMatrix);
	XMStoreFloat4x4(&mScenePass->lightViewProj, XMMatrixTranspose(mLightViewMtrix * mLightProjectionMatrix));
	

}
