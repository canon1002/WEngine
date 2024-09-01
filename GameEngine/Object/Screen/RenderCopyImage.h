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

	Vector4 vignettingColor;		// 画面端の色を設定する

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

class RenderCopyImage{

private:

	RenderCopyImage();
	~RenderCopyImage();

public:
	
	static RenderCopyImage* GetInstance();
	void Finalize();

	void Initialize(DirectXCommon* dxCommon, CameraCommon* camera);
	void Update();
	void PreDraw();
	void Draw();

	// ビネットの色を変更
	void SetViggnetColor(Vector4 color) { fullScreenData->vignettingColor = color; }
	// ビネットの
	void SetViggnetMultiplier(float multiplier) { fullScreenData->vigneMultipliier = multiplier; }
	// ビネットの乗数を変更
	void SetViggnetIndex(float index) { fullScreenData->vigneIndex = index; }

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
	void CreateTransformation();
	void CreateBufferView();

public:

	/// <summary>
	///	座標変更
	/// </summary>
	/// <param name="pos">座標</param>
	void SetTransform(Vector3 pos) {
		mWorldTransform.translation = pos;
	}

	/// <summary>
	/// 中心座標を移動させる
	/// </summary>
	/// <param name="t">移動量</param>
	void Transform(Vector3 t) {
		mWorldTransform.translation.x += t.x;
		mWorldTransform.translation.y += t.y;
		mWorldTransform.translation.z += t.z;
	}

	/// <summary>
	/// 回転させる
	/// </summary>
	/// <param name="r">回転量</param>
	void Rotation(Vector3 r) {
		mWorldTransform.rotation.x += r.x;
		mWorldTransform.rotation.y += r.y;
		mWorldTransform.rotation.z += r.z;
	}

	/// <summary>
	/// 色変更
	/// </summary>
	/// <param name="color"></param>
	void SetColor(Color color) {
		// 指定した色に書き込む
		*materialData = Color(color.r, color.g, color.b, color.a);
	}

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return mVertexBufferView; }
	auto* GetMaterial() { return  fullScreenResource.Get(); }
	auto* GetWVP() { return mWvpResource.Get(); }



private:

	// 外部ポインタ
	CameraCommon* mCamera = nullptr;
	DirectXCommon* mDxCommon = nullptr;

	// グラフィックパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState = nullptr;
	// ルートシグネチャー
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature = nullptr;

	WorldTransform mWorldTransform;
	Matrix4x4 worldM, cameraM, viewM, projectM, pespectiveM, wvpM;
	Vector4 translate_;

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexResource = nullptr;
	// マテリアル用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> fullScreenResource = nullptr;
	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mWvpResource = nullptr;
	// データを書き込む
	Matrix4x4* mWvpData = nullptr;
	// 頂点リソースにデータを書き込む
	VertexData* mVertexData = nullptr;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};
	// マテリアルデータ
	Color* materialData = nullptr;
	// PostEffectデータ
	FullScereenEffect* fullScreenData = nullptr; 
	
	// テクスチャハンドル
	int32_t mTextureHandle;
	// depStencilResourceの登録番号とリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilResource;
	int32_t mDepthStencilHandle;

	// インスタンス
	static RenderCopyImage* instance;
};

