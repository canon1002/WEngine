#include "SceneManager.h"

// コンストラクタ
SceneManager::SceneManager() {

	// WindowsAPI
	win = WinAPI::GetInstance();
	// DirectX
	dx = DirectXCommon::GetInstance();
	// Input
	input = Input::GetInstance();
	// Audio
	audio = Audio::GetInstance();
	// メインカメラ
	mainCamera = MainCamera::GetInstance();
	// モデルマネージャー
	modelManager = ModelManager::GetInstance();

	// 各シーンの配列
	sceneArr_[TITLE] = std::make_unique<TitleScene>();
	sceneArr_[STAGE] = std::make_unique<GameMainScene>();
	sceneArr_[CLEAR] = std::make_unique<ResultScene>();

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
	input->Initialize();
	audio->Initialize();
	mainCamera->Initialize({ {1.0f,1.0f,1.0f},{0.314f,-0.314f,0.0f},{2.0f,2.0f,-6.0f} });
	//mainCamera->Initialize({ {1.0f,1.0f,1.0f},{0.314f,0.0f,0.0f},{0.0f,16.0f,-48.0f} });
	modelManager->Initialize();

	while (true)	{

		// Windowsのメッセージ処理
		if (win->ProcessMessage()) {
			// ゲームループを抜ける
			break;
		}

		// フレームの先頭でImGuiに、ここからフレームが始まる旨を伝える
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// 入力処理の更新を行う
		input->Update();
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

		///
		/// 更新処理(推定)
		///

		// 開発用UIの表示
		//ImGui::ShowDemoWindow();
		/// 更新処理
		sceneArr_[currentSceneNo_]->Update();
		// フレームレートの表示
		ImGui::Text("FPS : %.2f", ImGui::GetIO().Framerate);
		// 描画処理に入る前に、ImGui内部のコマンドを生成する
		ImGui::Render();

		///
		/// 描画処理(推定) 
		/// 

		// 描画前処理
		dx->DrawBegin();
		/// 描画処理
		sceneArr_[currentSceneNo_]->Draw();
	
		// 描画後処理
		dx->DrawEnd();

		// ESCキーが押されたらループを抜ける
		if (input->GetTriggerKey(DIK_ESCAPE)) {
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
	ImGui_ImplDX12_Shutdown();
	modelManager->Finalize();
	mainCamera->Finalize();
	input->Finalize();
	audio->Finalize();
	dx->Finalize();
	win->Finalize();

	return 0;
}