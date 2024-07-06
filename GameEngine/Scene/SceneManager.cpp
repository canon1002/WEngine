#include "SceneManager.h"

// コンストラクタ
SceneManager::SceneManager() {}
// デストラクタ
SceneManager::~SceneManager() {}

// 初期化
void SceneManager::Init(WinAPI* winApp, DirectXCommon* dxCommon){
	// ポインタを取得
	winApp_ = winApp;
	mDxCommon = dxCommon;

#ifdef _DEBUG
	// ImGuiManager
	imGuiManager_ = std::make_unique<ImGuiManager>();
#endif // _DEBUG

	// Input
	inputManager_ = InputManager::GetInstance();
	// Audio
	audio_ = Audio::GetInstance();
	// メインカメラ
	mainCamera_ = std::make_unique<MainCamera>();
	
	// オブジェクト管理者クラス
	objectAdmin_ = ObjectAdministrator::GetInstance();

	// ポストエフェクト管理者クラス
	mPostEffectManager = std::make_unique<PostEffectManager>();
	mPostEffectManager->Init();

	// 各シーンの配列
	sceneArr_[TITLE] = std::make_unique<TitleScene>();
	sceneArr_[STAGE] = std::make_unique<GameScene>();
	sceneArr_[CLEAR] = std::make_unique<ResultScene>();
	//sceneArr_[OVER] = std::make_unique<ResultScene>();

	// 初期シーン
	currentSceneNo_ = STAGE;

	//
	copyImage_ = std::make_unique<RenderCopyImage>();
	copyImage_->Initialize(DirectXCommon::GetInstance(), MainCamera::GetInstance());
}

// 処理
int SceneManager::Run() {
	

#ifdef _DEBUG
	imGuiManager_->Initialize(winApp_, mDxCommon);
#endif // _DEBUG


	inputManager_->Initialize(winApp_);
	audio_->Initialize();
	mainCamera_->Initialize(winApp_);
	//mainCamera_->mWorldTransform->translation = { 0.0f,0.0f,-6.0f };
	
	objectAdmin_->Init(mDxCommon);


	// Windowsのメッセージ処理があればゲームループを抜ける
	while (!winApp_->ProcessMessage())	{

		// ImGuiの入力を受け付ける
#ifdef _DEBUG
		imGuiManager_->Begin();
#endif // _DEBUG
		// 入力処理の更新を行う
		inputManager_->Update();
		// シーンのチェック
		currentSceneNo_ = sceneArr_[currentSceneNo_]->GetSceneNo();
		// シーン変更チェック
		if (prevSceneNo_ != currentSceneNo_) {
			sceneArr_[currentSceneNo_]->Init();
		}
		// 前回のシーン番号を上書き
		prevSceneNo_ = currentSceneNo_;

		// カメラの更新
		//mainCamera_->Update();
		// 入力結果をImGuiで表示する
		//inputManager_->DrawGUI();

		///
		/// 更新処理(推定)
		///

		/// 更新処理
		sceneArr_[currentSceneNo_]->Update();
		
		//mPostEffectManager->Update();
		
		// ポストエフェクト
		copyImage_->Update();
		
		#ifdef _DEBUG
		// 開発用UIの表示
		//ImGui::ShowDemoWindow();
		// フレームレートの表示
		ImGui::Text("FPS : %.2f", ImGui::GetIO().Framerate);
		imGuiManager_->End();
		#endif // _DEBUG

		///
		/// 描画処理(推定) 
		/// 

		// 描画前処理 -- RenderTexture --
		mDxCommon->PreDrawForRenderTarget();

		/// 描画処理
		sceneArr_[currentSceneNo_]->Draw();

		// 描画後処理 -- RenderTexture --
		mDxCommon->PostDrawForRenderTarget();

		// 描画前処理
		mDxCommon->PreDraw();

		// ポストエフェクトの積み込み
		//mPostEffectManager->Draw();

		copyImage_->PreDraw();
		copyImage_->Draw();

		// ポストエフェクトの描画
		//mPostEffectManager->RenderFinalOutput();

#ifdef _DEBUG
		// ImGuiの描画
		imGuiManager_->Draw();
#endif // _DEBUG

		// 描画後処理
		mDxCommon->PostDraw();

		// ESCキーが押されたらループを抜ける
		if (inputManager_->GetKey()->GetTriggerKey(DIK_ESCAPE)) {
			break;
		}

	}

	// Comの終了処理
	CoUninitialize();

	// 解放処理
	sceneArr_[TITLE]->Finalize();
	sceneArr_[STAGE]->Finalize();
	sceneArr_[CLEAR]->Finalize();
	sceneArr_[TITLE].reset();
	sceneArr_[STAGE].reset();
	sceneArr_[CLEAR].reset();
	
	inputManager_->Finalize();
	audio_->Finalize();

	return 0;
}