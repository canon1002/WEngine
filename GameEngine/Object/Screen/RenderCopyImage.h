#pragma once
#include "GameEngine/Math/Math.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Append/Transform/WorldTransform.h"

// 前方宣言
class CameraCommon;

// レンダリング時にポストエフェクトで使う変数をまとめた構造体
struct FullScereenEffect {
	int32_t enableScreenColor;  // 画面全体の色を変更する
	int32_t enableGrayScele;	// Graysceleの有無
	int32_t padding[2];

	Vector4 screenColor;     // 上記の際に使うVector4(RGB+A型)

	int32_t enableVignetting;   // ビネット処理の有無(画面端を暗くする)
	float vigneMultipliier; // ビネット処理の際に使用する乗数
	float vigneIndex;       // ビネット処理の際に使用する指数
	int32_t enableSmooting;     // Smooting(ぼかし)の有無 (ぼかしの種類は以下の変数で決める)
	
	int32_t enableBoxFilter;    // ぼかしの際にBoxFillterを使用するのか
	int32_t enableGaussianFilter;    // ぼかしをガウスぼかしにするのか
	int32_t kernelSize;       // カーネルの大きさ
	float GaussianSigma;    // GaussianFilterの際に使う標準偏差

	int32_t enableLuminanceOutline; // 輝度で検出したアウトラインの有無
	float outlineMultipliier;   // アウトライン生成時の差を大きくするための数値  
	int32_t enableDepthOutline; // 深度(Depth)で検出したアウトラインの有無
	int32_t padding02[1];

	Matrix4x4 projectionInverse; // NDCをViewに変換するために使う逆行列    
};

struct EffectFlags {
	bool isEnableScreenColor;
	bool isEnableViggetting;
	bool isEnableSmooting;
	bool isEnableBoxFilter;
	bool isEnableGaussianFilter;
	bool isEnebleLuminanceOutline;
	bool isEnableDepthOutline;
};

class RenderCopyImage{

public:
	RenderCopyImage();
	~RenderCopyImage();

	void Initialize(DirectXCommon* dxCommon, CameraCommon* camera);
	void Update();
	void PreDraw();
	void Draw();



private:	// -- private メンバ関数 -- //

	/// <summary>
	/// 3Dモデル用のルートシグネチャを生成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// 3Dモデル用のグラフィックスパイプラインを生成
	/// </summary>
	void CreateGraphicsPipeline();

	void CreateVertexResource();
	void CreateTransformationRsource();
	void CreateBufferView();

public:

	/// <summary>
	///	座標変更
	/// </summary>
	/// <param name="pos">座標</param>
	void SetTransform(Vector3 pos) {
		worldTransform_.translation = pos;
	}

	/// <summary>
	/// 中心座標を移動させる
	/// </summary>
	/// <param name="t">移動量</param>
	void Transform(Vector3 t) {
		worldTransform_.translation.x += t.x;
		worldTransform_.translation.y += t.y;
		worldTransform_.translation.z += t.z;
	}

	/// <summary>
	/// 回転させる
	/// </summary>
	/// <param name="r">回転量</param>
	void Rotation(Vector3 r) {
		worldTransform_.rotation.x += r.x;
		worldTransform_.rotation.y += r.y;
		worldTransform_.rotation.z += r.z;
	}

	/// <summary>
	/// 色変更
	/// </summary>
	/// <param name="color"></param>
	void SetColor(Color color) {
		// 指定した色に書き込む
		*materialData = Color(color.r, color.g, color.b, color.a);
	}

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return vertexBufferView; }
	auto* GetMaterial() { return  fullScreenResource.Get(); }
	auto* GetWVP() { return wvpResource.Get(); }

private:

	// 外部ポインタ
	CameraCommon* camera_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature = nullptr;

	WorldTransform worldTransform_;
	Matrix4x4 worldM, cameraM, viewM, projectM, pespectiveM, wvpM;
	Vector4 translate_;

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	// マテリアル用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> fullScreenResource = nullptr;
	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = nullptr;
	// データを書き込む
	Matrix4x4* wvpData = nullptr;
	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// マテリアルデータ
	Color* materialData = nullptr;
	// PostEffectデータ
	FullScereenEffect* fullScreenData = nullptr; 
	// フラグ(これはHLSL関連に送らない)
	EffectFlags effectFlags;
	// テクスチャハンドル
	int32_t textureHandle_;
	// depStencilResourceの登録番号とリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilResource;
	int32_t mDepthStencilHandle;
};

