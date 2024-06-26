#pragma once
#include "GameEngine/Base/WinApp/WinAPI.h"

class DirectXCommon;

class DSV {
public: // -- public メンバ関数 -- //

	DSV() = default;
	~DSV() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon);

private: // -- private メンバ関数 -- //

	/// <summary>
	/// ディープステンシルビューの生成
	/// </summary>
	void CreateDepthStencilView();

	// コピーコンストラクタと演算子オーバーロードの禁止
	DSV(const DSV& obj) = delete;
	DSV& operator=(const DSV& obj) = delete;

public: // -- public メンバ変数 -- //

	// DSV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;
	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	//　ディープステンシル用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilTextureResource_ = nullptr;
	//　ディスクリプタ
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;

private: // -- private メンバ変数 -- //

	// ポインタ
	DirectXCommon* dxCommon_;

};