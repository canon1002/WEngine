#include "SceneManager.h"
#include "GameEngine/Editor/GlobalVariables.h"
#include "GameEngine/Editor/LevelEditor.h"
#include "GameEngine/GameBase/Reaction/DamageReaction.h"

#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>

#include "GameEngine/GameMaster/Framerate.h"

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
	mainCamera_->Initialize();

	// オブジェクト管理者クラス
	objectAdmin_ = ObjectAdministrator::GetInstance();
	objectAdmin_->Init(mDxCommon);

	// モデル管理クラス
	ModelManager::GetInstance()->Initialize(dxCommon, mainCamera_.get());

	// グローバル変数読み込み
	GlobalVariables::GetInstance()->LoadFiles();
	// レベルデータ読み込み
	LevelEditor::GetInstance()->CheckLevelEditorFile();
	DamageReaction::GetInstance()->Init();

	// 各シーンの配列
	sceneArr_[TITLE] = std::make_unique<TitleScene>();
	sceneArr_[STAGE] = std::make_unique<GameScene>();
	sceneArr_[RESULT] = std::make_unique<ResultScene>();
	//sceneArr_[OVER] = std::make_unique<ResultScene>();

	// 初期シーン
	currentSceneNo_ = STAGE;

	//
	copyImage_ = RenderCopyImage::GetInstance();
	copyImage_->Initialize(DirectXCommon::GetInstance(), MainCamera::GetInstance());
}

// 処理
int SceneManager::Run() {
	

#ifdef _DEBUG
	imGuiManager_->Initialize(winApp_, mDxCommon);
#endif // _DEBUG


	inputManager_->Initialize(winApp_);
	audio_->Initialize();
	//mainCamera_->mWorldTransform->translation = { 0.0f,0.0f,-6.0f };

	// 非同期処理
	//std::mutex mutex;
	//std::condition_variable condition; // 条件変数
	//std::queue<int> q;
	//bool exit = false;

	// バックグラウンドループ
	//std::thread th([&]() {
	//	while (!exit){
	//		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	//		// 以下 排他処理範囲を示す
	//		{
	//			std::unique_lock<std::mutex> uniqueLock(mutex);
	//			// qに何かしらのデータが入っていればq.popを実行する
	//			condition.wait(uniqueLock, [&]() {return !q.empty(); });
	//			q.pop();
	//		}
	//	}

	//	});

	// Windowsのメッセージ処理があればゲームループを抜ける
	while (!winApp_->ProcessMessage())	{

		// ImGuiの入力を受け付ける
#ifdef _DEBUG
		imGuiManager_->Begin();
#endif // _DEBUG

		// 入力処理の更新を行う
		inputManager_->Update();
		// グローバル変数の更新
		GlobalVariables::GetInstance()->Update();
		// シーンのチェック
		currentSceneNo_ = sceneArr_[currentSceneNo_]->GetSceneNo();
		// シーン変更チェック
		if (prevSceneNo_ != currentSceneNo_) { sceneArr_[currentSceneNo_]->Init(); }
		// 前回のシーン番号を上書き
		prevSceneNo_ = currentSceneNo_;

		///
		/// 更新処理(推定)
		///


		// コマ送り時に設定した経過フレームになっていない場合、
		// 更新処理をスキップする
		Framerate::GetInstance()->DrawGui();
		if (Framerate::GetInstance()->GetIsFrameAdvance()&&
			!Framerate::GetInstance()->IsActiveFrame()) {
		
		}
		else {
			/// 更新処理
			sceneArr_[currentSceneNo_]->Update();
			// ポストエフェクト
			copyImage_->Update();
			// ダメージ表記の更新
			DamageReaction::GetInstance()->UpdateSprite();

		}
		Framerate::GetInstance()->Update();

		// 開発用UIの表示
#ifdef _DEBUG

		// 画面上部にメインメニューバーを追加
		ImGui::BeginMainMenuBar();
		// フレームレートの表示
		//ImGui::Text("FPS : %.2f", ImGui::GetIO().Framerate);
		// レベルエディタ
		if(ImGui::Button("LoadLevelEditor")) {
			// ボタンを押すとファイルの再読み込みを行う
			LevelEditor::GetInstance()->CheckLevelEditorFile();
		}
		
		// バックグラウンド処理
		//if (ImGui::Button("Q")) {
		//	// ボタンを押すとバックグラウンド処理を行う
		//	std::unique_lock<std::mutex> uniquelock(mutex);
		//	q.push(1);
		//	// 通知を送る
		//	condition.notify_all();
		//}

		ImGui::EndMainMenuBar();
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
		
		copyImage_->PreDraw();
		copyImage_->Draw();

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

	// ゲームを抜けたらスレッドの占有を止める
	/*exit = true;
	th.join();*/

	// Comの終了処理
	CoUninitialize();

	// 解放処理
	sceneArr_[TITLE]->Finalize();
	sceneArr_[STAGE]->Finalize();
	sceneArr_[RESULT]->Finalize();
	sceneArr_[TITLE].reset();
	sceneArr_[STAGE].reset();
	sceneArr_[RESULT].reset();
	
	inputManager_->Finalize();
	audio_->Finalize();
	copyImage_->Finalize();

	return 0;
}