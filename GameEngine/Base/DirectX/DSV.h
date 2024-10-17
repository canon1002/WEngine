#pragma once
#include "Base/WinApp/WinApp.h"

class DirectXCommon;

class DSV {
public: // -- public メンバ関数 -- //

	DSV() = default;
	~DSV() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon"></param>
	void Init(DirectXCommon* dxCommon);

private: // -- private メンバ関数 -- //

	/// <summary>
	/// ディープステンシルビューの生成
	/// </summary>
	void CreateDepthStencilView();

	/// <summary>
	/// 深度バッファ生成
	/// </summary>
	void CreateDepthBuffer();

	// コピーコンストラクタと演算子オーバーロードの禁止
	DSV(const DSV& obj) = delete;
	DSV& operator=(const DSV& obj) = delete;

public: // -- public メンバ変数 -- //

	// DSV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvDescriptorHeap = nullptr;
	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC mDsvDesc{};
	//　ディープステンシル用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilTextureResource = nullptr;
	//　ディスクリプタ
	D3D12_CPU_DESCRIPTOR_HANDLE mDsvHandle;

	// 深度バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthBuffer;

private: // -- private メンバ変数 -- //

	// ポインタ
	DirectXCommon* mDxCommon;

};