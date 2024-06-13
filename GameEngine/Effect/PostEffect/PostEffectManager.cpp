#include "PostEffectManager.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GrayScale.h"
#include "Vignette.h"
#include "BoxFilter.h"
#include "GaussianFilter.h"
#include "LuminanceOutline.h"


PostEffectManager::~PostEffectManager(){

}

void PostEffectManager::Init(){
	// 各派生クラスを配列に追加する
	mPostEffects.push_back(new Grayscale()); // グレースケール
	mPostEffects.push_back(new Vignette()); // ビネット
	mPostEffects.push_back(new BoxFilter()); // BoxFilter
	mPostEffects.push_back(new GaussianFilter()); // ガウスぼかし
	mPostEffects.push_back(new LuminanceOutline()); // 輝度検出アウトライン

	// 追加した派生クラスを初期化する
	for (const auto& postEffect : mPostEffects) {
		postEffect->Init();
	}
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

}

void PostEffectManager::Draw() {
	for (int32_t i = 0; i < mPostEffects.size(); ++i) {
		mPostEffects[i]->PreDraw();
		mPostEffects[i]->Draw();
		mPostEffects[i]->PostDraw();
	}
}
