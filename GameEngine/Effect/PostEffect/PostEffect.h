#pragma once
#include "GameEngine/Utility/Math/Math.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Utility/Transform/WorldTransform.h"

// 前方宣言
class CameraCommon;

// 各エフェクトごとの要素別に構造体でまとめておく

// スクリーン
struct Screen{
	int32_t enableGrayScele;	// Graysceleの有無
	int32_t enableScreenColor;  // 画面全体の色を変更する
	int32_t padding[2];			// *メモリ合わせ
	Vector4 screenColor;		// 上記の際に使うVector4(RGB+A型)
};


// ビネット
struct Vignette{
	int32_t enable;		// 使用フラグ
	float index;		// 乗数
	float multipliier;	// 指数
	int32_t padding;	// *メモリ合わせ
	Vector4 color;		// ビネットの色
};

// ぼかし
struct Smooting {
	int32_t useBox;			// BoxFillterを使用する
	int32_t useGaussian;    // GaussianFillterを使用する
	int32_t kernelSize;     // カーネルの大きさ
	float gaussianSigma;    // GaussianFilterの際に使う標準偏差
};

// 輝度アウトライン
struct LuminanceOutline {
	int32_t  enable;	// 使用フラグ
	float multipliier;  // アウトライン生成時の差を大きくする数値 
};

// 深度アウトライン
struct DepthOutline {
	int32_t  enable;	// 使用フラグ
	Matrix4x4 projectionInverse; // NDCをViewに変換するために使う逆行列    
};

//	アウトライン
struct Outline {
	LuminanceOutline luminance;	// 輝度アウトライン
	int32_t padding;	// *メモリ合わせ
	DepthOutline depth;		// 深度アウトライン
};

// レンダリング時のポストエフェクト
struct PostEffects {
	Screen screen;		// スクリーンの色を変更する
	Vignette vignette;	// ビネット
	Vignette redVignnete; // 赤色ビネット
	Smooting smooting;	// ぼかし
	Outline outline;	// アウトライン
};

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

class PostEffect{

private:

	PostEffect();
	~PostEffect();

public:
	
	static PostEffect* GetInstance();
	void Finalize();

	void Init();
	void Update();
	void PreDraw();
	void Draw();
	void Debug();

	// ビネットの色を変更
	void SetViggnetColor(Vector4 color) { mPostEffects->vignette.color = color; }
	// ビネットの指数
	void SetViggnetMultiplier(float multiplier) { mPostEffects->vignette.multipliier = multiplier; }
	// ビネットの乗数を変更
	void SetViggnetIndex(float index) { mPostEffects->vignette.index = index; }

	// -- 以下 赤色ビネット 処理 -- //

	// ビネットの切り替え
	void SetRedViggnetEnable(bool flag) { mPostEffects->redVignnete.enable = flag; }

	// ビネットの色を変更
	void SetRedViggnetColor(Vector4 color) { mPostEffects->redVignnete.color = color; }
	// ビネットの指数
	void SetRedViggnetMultiplier(float multiplier) { mPostEffects->redVignnete.multipliier = multiplier; }
	// ビネットの乗数を変更
	void SetRedViggnetIndex(float index) { mPostEffects->redVignnete.index = index; }


private:	// -- private メンバ関数 -- //

	/// <summary>
	/// 3Dモデル用のルートシグネチャを生成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// 3Dモデル用のグラフィックスパイプラインを生成
	/// </summary>
	void CreateGraphicsPipeline();

	/// <summary>
	/// 頂点リソースの生成
	/// </summary>
	void CreateVertexResource();

	/// <summary>
	/// 座標リソースの生成
	/// </summary>
	void CreateTransformation();


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
	auto* GetMaterial() { return  mPostEffectResource.Get(); }
	auto* GetWVP() { return mWvpResource.Get(); }



private:

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
	// ポストエフェクトのパラメータ用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mPostEffectResource = nullptr;
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
	//FullScereenEffect* fullScreenData = nullptr; 
	PostEffects* mPostEffects;

	// テクスチャハンドル
	int32_t mTextureHandle;

	// depStencilResourceの登録番号とリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilResource;
	int32_t mDepthStencilHandle;

	// インスタンス
	static PostEffect* instance;
};

