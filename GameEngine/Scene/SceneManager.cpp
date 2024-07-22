#include "SceneManager.h"
#include "GameEngine/Editor/GlobalVariables.h"
#include "GameEngine/Editor/LevelEditor.h"
#include "GameEngine/GameBase/Reaction/DamageReaction.h"

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
	mainCamera_->Initialize(winApp_);

	// オブジェクト管理者クラス
	objectAdmin_ = ObjectAdministrator::GetInstance();
	objectAdmin_->Init(mDxCommon);

	// モデル管理クラス
	ModelManager::GetInstance()->Initialize(dxCommon, mainCamera_.get());


	// ポストエフェクト管理者クラス
	mPostEffectManager = std::make_unique<PostEffectManager>();
	mPostEffectManager->Init();

	// グローバル変数読み込み
	GlobalVariables::GetInstance()->LoadFiles();
	// レベルデータ読み込み
	LevelEditor::GetInstance()->CheckLevelEditorFile();
	DamageReaction::GetInstance()->Init();

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
	//mainCamera_->mWorldTransform->translation = { 0.0f,0.0f,-6.0f };


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
		mPostEffectManager->Update();
		copyImage_->Update();
		#ifdef _DEBUG
		// 開発用UIの表示
		//ImGui::ShowDemoWindow();
		ImGui::BeginMainMenuBar();
		// フレームレートの表示
		//ImGui::Text("FPS : %.2f", ImGui::GetIO().Framerate);
		if(ImGui::Button("LoadLevelEditor")) {
			LevelEditor::GetInstance()->CheckLevelEditorFile();
		}

		if (ImGui::Button("SetDamage")) {
			DamageReaction::GetInstance()->Reaction(Vector3(2.0f, 2.0f, 10.0f), 147, mainCamera_.get());
		}
		ImGui::EndMainMenuBar();
		imGuiManager_->End();
		#endif // _DEBUG

		DamageReaction::GetInstance()->UpdateSprite();

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

		mPostEffectManager->Draw();

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