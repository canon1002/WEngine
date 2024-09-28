#pragma once
#include "Base/DirectX/DirectXCommon.h"

using namespace DirectX;

struct ScenePassConstants {
	XMFLOAT4X4 view;        // ビュー行列
	XMFLOAT4X4 proj;        // プロジェクション行列
	XMFLOAT4X4 viewProj;    // ビュー・プロジェクション行列
	XMFLOAT4X4 lightViewProj; // ライトのビュー・プロジェクション行列
	// 他のシーンパス関連の定数
};

class ShadowMap
{
public:

	ShadowMap() = default;
	~ShadowMap() = default;

	void Init();
	void Upadate();
	void Draw();

	///
	void CreateScenePass();


public:

	// カメラ
	XMVECTOR eyePos;  // カメラの位置
	XMVECTOR target;  // カメラの注視点
	XMVECTOR up;      // カメラの上方向
	XMMATRIX viewMatrix;
	float fovAngleY;  // 垂直方向の視野角（ラジアン）
	float aspectRatio; // アスペクト比
	float nearZ;  // 近クリップ面
	float farZ; // 遠クリップ面

	XMMATRIX projectionMatrix;

	// ライト
	XMVECTOR mLightPos;		// 位置
	XMVECTOR mLightTarget;	// 対象
	XMVECTOR mLightUp;
	XMMATRIX mLightViewMtrix; // ビュー行列
	float nearPlane;
	float farPlane;
	float fieldOfView;
	XMMATRIX mLightProjectionMatrix;// プロジェクション行列
	


	Microsoft::WRL::ComPtr<ID3D12Resource> mScenePassResource;
	ScenePassConstants* mScenePass;




};

