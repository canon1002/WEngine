#pragma once
#include "GameEngine/Base/WinApp/WinAPI.h"
#include "GameEngine/Utility/Math/Math.h"

class DirectXCommon;

class RTV{
public: // -- public メンバ関数 -- //

	RTV() = default;
	~RTV() = default;

	/// 初期化
	void Init();

private: // -- private メンバ関数 -- //

	/// <summary>
	/// レンダーターゲット生成
	/// </summary>
	void CreateFinalRenderTargets();

	// コピーコンストラクタと演算子オーバーロードの禁止
	RTV(const RTV& obj) = delete;
	RTV& operator=(const RTV& obj) = delete;

public: // -- public メンバ変数 -- //

	// RTVを2つ作るのでディスクリプタを２つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[3];
	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	// RTV用ディスクリプタヒープ
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> rtvDescriptorHeap = nullptr;
	// RenderTextureで初期化する際の色
	const float kRenderTargetColor[4] = { 0.2f,0.2f,0.2f,1.0f };

	Microsoft::WRL::ComPtr <ID3D12Resource> mRenderTextureResource = nullptr;
	Microsoft::WRL::ComPtr <ID3D12Resource> mIntermediateRenderTextureh = nullptr;

private: // -- private メンバ変数 -- //


};