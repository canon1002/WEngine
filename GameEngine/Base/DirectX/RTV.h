#pragma once
#include "GameEngine/Base/WinApp/WinAPI.h"
#include "GameEngine/Math/Math.h"

class DirectXCommon;

class RTV{
public: // -- public メンバ関数 -- //

	RTV() = default;
	~RTV() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon);

private: // -- private メンバ関数 -- //

	/// <summary>
	/// レンダーターゲット生成
	/// </summary>
	void CreateFinalRenderTargets();

	// レンダーテクスチャの生成
	Microsoft::WRL::ComPtr <ID3D12Resource> CreateRenderTextureResource(
		Microsoft::WRL::ComPtr <ID3D12Device> device, uint32_t width, uint32_t height,
		DXGI_FORMAT format, const Vector4& clearColor
	);

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

private: // -- private メンバ変数 -- //

	// ポインタ
	DirectXCommon* dxCommon_;

};