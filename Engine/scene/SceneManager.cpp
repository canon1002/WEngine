#include "SceneManager.h"

// コンストラクタ
SceneManager::SceneManager() {

	// WindowsAPI
	win = WinAPI::GetInstance();
	// DirectX
	dx = DirectXCommon::GetInstance();
#ifdef _DEBUG
	// ImGuiManager
	imGuiManager_ = ImGuiManager::GetInstance();
#endif // _DEBUG

	// Input
	inputManager = InputManager::GetInstance();
	// Audio
	audio = Audio::GetInstance();
	// メインカメラ
	mainCamera = MainCamera::GetInstance();
	// モデルマネージャー
	modelManager = ModelManager::GetInstance();

	// スプライト基盤クラス
	spriteCommon = SpriteCommon::GetInstance();

	// 各シーンの配列
	sceneArr_[TITLE] = std::make_unique<TitleScene>();
	sceneArr_[STAGE] = std::make_unique<GameMainScene>();
	sceneArr_[CLEAR] = std::make_unique<ResultScene>();
	//sceneArr_[OVER] = std::make_unique<ResultScene>();

	// 初期シーン
	currentSceneNo_ = STAGE;

}

// デストラクタ
SceneManager::~SceneManager() {}

// 処理
int SceneManager::Run() {

	// 初期化
	win->Initialize();
	dx->Initialize(win);
#ifdef _DEBUG
	imGuiManager_->Initialize();
#endif // _DEBUG
	inputManager->Initialize();
	audio->Initialize();
	mainCamera->Initialize({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-6.0f} });
	//mainCamera->Initialize({ {1.0f,1.0f,1.0f},{0.314f,0.0f,0.0f},{0.0f,16.0f,-48.0f} });
	modelManager->Initialize();
	spriteCommon->Initialize();

	// Windowsのメッセージ処理があればゲームループを抜ける
	while (!win->ProcessMessage())	{

		// ImGuiの入力を受け付ける
		imGuiManager_->Begin();
		// 入力処理の更新を行う
		inputManager->Update();
		// シーンのチェック
		currentSceneNo_ = sceneArr_[currentSceneNo_]->GetSceneNo();
		// シーン変更チェック
		if (prevSceneNo_ != currentSceneNo_) {
			sceneArr_[currentSceneNo_]->Init();
		}
		// 前回のシーン番号を上書き
		prevSceneNo_ = currentSceneNo_;

		// カメラの更新
		mainCamera->Update();
		// 入力結果をImGuiで表示する
		inputManager->DrawGUI();

		///
		/// 更新処理(推定)
		///

		/// 更新処理
		sceneArr_[currentSceneNo_]->Update();
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

		// 描画前処理
		dx->DrawBegin();
		/// 描画処理
		sceneArr_[currentSceneNo_]->Draw();
	
#ifdef _DEBUG
		// ImGuiの描画
		imGuiManager_->Draw();
#endif // _DEBUG

		// 描画後処理
		dx->DrawEnd();

		// ESCキーが押されたらループを抜ける
		if (inputManager->GetKey()->GetTriggerKey(DIK_ESCAPE)) {
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
	

	spriteCommon->Finalize();
	modelManager->Finalize();
	mainCamera->Finalize();
	inputManager->Finalize();
	audio->Finalize();
	dx->Finalize();
	win->Finalize();

	return 0;
}