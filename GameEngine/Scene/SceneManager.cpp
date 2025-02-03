#include "SceneManager.h"
#include "GameEngine/Editor/GlobalVariables.h"
#include "GameEngine/Editor/LevelEditor.h"
#include "App/Reaction/DamageReaction.h"

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
void SceneManager::Init(){

#ifdef _DEBUG
	// ImGuiManager
	imGuiManager_ = std::make_unique<ImGuiManager>();
#endif // _DEBUG

	
	// メインカメラ
	MainCamera::GetInstance()->Initialize();

	// オブジェクト管理者クラス
	ObjectManager::GetInstance()->Init();

	// モデル管理クラス
	ModelManager::GetInstance()->Init();

	// グローバル変数読み込み
	GlobalVariables::GetInstance()->LoadFiles();
	// レベルデータ読み込み
	LevelEditor::GetInstance()->CheckLevelEditorFile();
	DamageReaction::GetInstance()->Init();

	// 各シーンの配列
	sceneArr_[START] = std::make_unique<StartScene>();
	sceneArr_[TITLE] = std::make_unique<TitleScene>();
	sceneArr_[STAGE] = std::make_unique<GameScene>();
	sceneArr_[RESULT] = std::make_unique<ResultScene>();
	sceneArr_[OVER] = std::make_unique<OverScene>();

	// フレームレート
	Framerate::GetInstance()->Init();

	// ポストエフェクト
	PostEffect::GetInstance()->Init();
}

// 処理
int SceneManager::Run() {
	

#ifdef _DEBUG
	imGuiManager_->Init();
#endif // _DEBUG

	// 入力マネージャ
	InputManager::GetInstance()->Init();
	// Audio
	Audio::GetInstance()->Initialize();

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
	while (!WinApp::GetInstance()->ProcessMessage()) {

		// ImGuiの入力を受け付ける
#ifdef _DEBUG
		imGuiManager_->Begin();
#endif // _DEBUG

		// 入力処理の更新を行う
		InputManager::GetInstance()->Update();
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
			PostEffect::GetInstance()->Update();
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
		
		// ポストエフェクトのImGui
		PostEffect::GetInstance()->Debug();

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
		DirectXCommon::GetInstance()->PreDrawForRenderTarget();
		/// 描画処理
		sceneArr_[currentSceneNo_]->Draw();

		// 描画後処理 -- RenderTexture --
		DirectXCommon::GetInstance()->PostDrawForRenderTarget();
		// 描画前処理
		DirectXCommon::GetInstance()->PreDraw();
		
		PostEffect::GetInstance()->PreDraw();
		PostEffect::GetInstance()->Draw();

		// UI描画
		sceneArr_[currentSceneNo_]->DrawUI();

#ifdef _DEBUG
		// ImGuiの描画
		imGuiManager_->Draw();
#endif // _DEBUG

		// 描画後処理
		DirectXCommon::GetInstance()->PostDraw();

		// ESCキーが押されたらループを抜ける
		if (InputManager::GetInstance()->GetKey()->GetTriggerKey(DIK_ESCAPE)) {
			break;
		}

	}

	// ゲームを抜けたらスレッドの占有を止める
	/*exit = true;
	th.join();*/

	// Comの終了処理
	CoUninitialize();

	// 解放処理
	sceneArr_[START]->Finalize();
	sceneArr_[TITLE]->Finalize();
	sceneArr_[STAGE]->Finalize();
	sceneArr_[RESULT]->Finalize();
	sceneArr_[OVER]->Finalize();

	sceneArr_[TITLE].reset();
	sceneArr_[STAGE].reset();
	sceneArr_[RESULT].reset();
	
	InputManager::GetInstance()->Final();
	Audio::GetInstance()->Finalize();
	PostEffect::GetInstance()->Finalize();

	return 0;
}