#include "PostEffectManager.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GrayScale.h"
#include "Vignette.h"
#include "BoxFilter.h"
#include "GaussianFilter.h"
#include "LuminanceOutline.h"
#include "DepthOutline.h"

PostEffectManager::~PostEffectManager(){

}

void PostEffectManager::Init(){
	// DirectXCommonのポインタを取得
	mDxCommon = DirectXCommon::GetInstance();

	// 各派生クラスを配列に追加する
	mPostEffects.push_back(new Grayscale()); // グレースケール
	mPostEffects.push_back(new Vignette()); // ビネット
	mPostEffects.push_back(new BoxFilter()); // BoxFilter
	mPostEffects.push_back(new GaussianFilter()); // ガウスぼかし
	mPostEffects.push_back(new LuminanceOutline()); // 輝度検出アウトライン
	//mPostEffects.push_back(new DepthOutline()); // 深度 アウトライン

	// 追加した派生クラスを初期化する
	for (const auto& postEffect : mPostEffects) {
		postEffect->Init();
	}

	// 中間テクスチャの生成
	CreateIntermediateTextures();

	// 出力用クラスの宣言
	mFinalRender = new RenderImage();
	mFinalRender->Init();

}

void PostEffectManager::Update(){

	// デバッグ用のUIを表示
#ifdef _DEBUG
	ImGui::Begin("PostEffect");
	for (const auto& postEffect : mPostEffects) {
		postEffect->DrawGUI();
	}
	ImGui::End();
#endif // _DEBUG

	// 更新処理	
	for (const auto&postEffect : mPostEffects) {
		postEffect->Update();
	}
	mFinalRender->Update();
}

void PostEffectManager::Draw() {
	//// レンダーターゲットのリソースを取得
	//Microsoft::WRL::ComPtr<ID3D12Resource> currentInput = mDxCommon->rtv_->mRenderTextureResource.Get();

	for (int32_t i = 0; i < mPostEffects.size(); ++i) {
		// 入力テクスチャを設定
		//mPostEffects[i]->SetInputTexture(currentInput);
		
		// 描画前処理
		// ルートシグネチャやグラフィックスパイプラインなどをセットする
		// リソースバリア関連の処理をいれるべき...?
		mPostEffects[i]->PreDraw();
		
		// 描画
		// ここでレンダーターゲットに書き込みを行う
		mPostEffects[i]->Draw();

		// 描画後処理
		// 未設定。リソースバリア関連の処理をいれるべき...?
		mPostEffects[i]->PostDraw();
	}

}

void PostEffectManager::RenderFinalOutput() {
	mFinalRender->PreDraw();
	mFinalRender->Draw();
	mFinalRender->PostDraw();
}

void PostEffectManager::CreateIntermediateTextures() {
	// 中間テクスチャを2つ生成する
	for (int32_t i = 0; i < 2; i++) {
		mIntermediateTextures[i] = mDxCommon->srv_->CreateRenderTextureSRV(mDxCommon->rtv_->mRenderTextureResource.Get());
	}

}
