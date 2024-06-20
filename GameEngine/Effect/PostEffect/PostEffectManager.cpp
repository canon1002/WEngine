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
	// DirectXCommonのポインタを取得
	mDxCommon = DirectXCommon::GetInstance();

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

	// 中間テクスチャの生成
	CreateIntermediateTextures();

	// 出力用クラスの宣言
	//mFinalRender = new RenderImage();
	//mFinalRender->Init();

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
	//mFinalRender->Update();
}

void PostEffectManager::Draw() {
	//// レンダーターゲットのリソースを取得
	Microsoft::WRL::ComPtr<ID3D12Resource> currentInput = mDxCommon->rtv_->mRenderTextureResource.Get();

	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = mDxCommon->swapChain->GetCurrentBackBufferIndex();

	// TransitionBarrierの設定
	// 今回のバリアはトランジション
	mDxCommon->barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	mDxCommon->barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	mDxCommon->barrier.Transition.pResource = mDxCommon->swapChainResources[backBufferIndex].Get();
	// 遷移前(現在)のResourceState
	mDxCommon->barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	// 遷移後のResourceState
	mDxCommon->barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	// TransitionBarrierを張る
	mDxCommon->commandList->ResourceBarrier(1, &mDxCommon->barrier);


	// 描画先のRTVとDSVを設定する
	mDxCommon->commandList->OMSetRenderTargets(1,
		&mDxCommon->rtv_->rtvHandles[backBufferIndex], false, 
		&mDxCommon->dsv_->mDsvHandle);

	// 指定した深度で画面全体をクリアにする
	// フレームの最初に最も遠く(1.0)でクリアする
	//commandList->ClearDepthStencilView(dsv_->mDsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// 指定した色で画面全体をクリアにする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };	// 青っぽい色 RGBAの順
	mDxCommon->commandList->ClearRenderTargetView(mDxCommon->rtv_->rtvHandles[backBufferIndex],
		clearColor, 0, nullptr);

	// 描画用のディスクリプタヒープの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { mDxCommon->srv_->srvDescriptorHeap.Get() };
	mDxCommon->commandList->SetDescriptorHeaps(1, descriptorHeaps);

	// コマンドを積み込む
	mDxCommon->commandList->RSSetViewports(1, &mDxCommon->viewport);
	mDxCommon->commandList->RSSetScissorRects(1, &mDxCommon->scissorRect);

	for (int32_t i = 0; i < mPostEffects.size(); ++i) {
		// 入力テクスチャを設定
		mPostEffects[i]->SetInputTexture(currentInput);
		
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

	// TransitionBarrierの設定
	mDxCommon->barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	mDxCommon->barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	mDxCommon->barrier.Transition.pResource = mDxCommon->rtv_->mRenderTextureResource.Get();
	// 遷移前(現在)のResourceState
	mDxCommon->barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	// 遷移後のResourceState
	mDxCommon->barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	// TransitionBarrierを張る
	mDxCommon->commandList->ResourceBarrier(1, &mDxCommon->barrier);

}

void PostEffectManager::RenderFinalOutput() {
	//mFinalRender->PreDraw();
	//mFinalRender->Draw();
	//mFinalRender->PostDraw();
}

void PostEffectManager::CreateIntermediateTextures() {
	// 中間テクスチャを2つ生成する
	for (int32_t i = 0; i < 2; i++) {
		mIntermediateTextures[i] = mDxCommon->srv_->CreateRenderTextureSRV(mDxCommon->rtv_->mRenderTextureResource.Get());
	}

}
