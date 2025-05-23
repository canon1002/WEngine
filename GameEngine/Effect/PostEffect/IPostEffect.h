#pragma once
#include "GameEngine/Utility/Math/Math.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Utility/Transform/WorldTransform.h"
#include "GameEngine/Object/Camera/CameraCommon.h"

/// <summary>
/// ポストエフェクト 基底クラス
/// </summary>
class IPostEffect {
public:// -- 公開 メンバ関数 -- //

	IPostEffect() = default;
	~IPostEffect();
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void DrawGUI() = 0;
	virtual void PreDraw() = 0;
	virtual void Draw() = 0;
	virtual void PostDraw() = 0;
	virtual void CreateGraphicsPipeline() = 0;
	virtual void CreateRootSignature() = 0;
	
	// 中間テクスチャの入力
	inline void SetInputTexture(Microsoft::WRL::ComPtr<ID3D12Resource> inputTexture) {
		mInputTexture = inputTexture;
	}

	// 中間テクスチャの出力
	inline Microsoft::WRL::ComPtr<ID3D12Resource> GetOutputTexture() const {
		return mOutputTexture;
	}

protected: // -- 限定公開 メンバ関数 -- // // -- 以下 派生クラスの共通部分 -- //

	// 頂点リソース生成
	void CreateVertexResource();

protected: // -- 限定公開 メンバ変数 -- //
	
	// 外部ポインタ
	CameraCommon* mCamera = nullptr;
	

	// テクスチャハンドル 
	int32_t mTextureHandle;

	// 中間テクスチャ(入力)
	Microsoft::WRL::ComPtr<ID3D12Resource> mInputTexture = nullptr;
	// 中間テクスチャ(出力)
	Microsoft::WRL::ComPtr<ID3D12Resource> mOutputTexture = nullptr;

	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> mGraphicsPipelineState;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr <ID3D12RootSignature> mRootSignature;


	// 頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexResource = nullptr;
	// 頂点データ
	VertexData* mVertexData = nullptr;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};
};

