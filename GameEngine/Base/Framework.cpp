#include "Framework.h"
#include "GameEngine/Editor/GlobalVariables.h"
#include "GameEngine/Editor/LevelEditor.h"
#include "GameEngine/GameMaster/Framerate.h"

// リソース関係
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Resource/Audio/Audio.h"
#include "GameEngine/Effect/PostEffect/PostEffect.h"
#include "GameEngine/Resource/Texture/TextureManager.h"
#include "GameEngine/Effect/Particle/ParticleManager.h"

// アプリケーション部分(のうちエンジン側に移動する可能性のある要素)
//#include "App/Reaction/DamageReaction.h"


void Framework::Run(){

	// ゲームの初期化
	Init();

	// ゲームループ(Windowsのメッセージ処理があれば抜ける)
	while (!WinApp::GetInstance()->ProcessMessage()){

		// ImGuiの入力を受け付ける
		#ifdef _DEBUG
		mImGuiManager->Begin();
		#endif // _DEBUG

		// 更新処理
		Update();

		// ImGuiの入力受付を終了
		#ifdef _DEBUG
		mImGuiManager->End();
		#endif // _DEBUG

		// 終了リクエストが来たら抜ける
		if (IsEndRequest()) {
			break;
		}
		// 描画処理
		Draw();
	}

	// 終了処理
	Final();

}

void Framework::Init(){
	
	// WinApp
	mWinApp = WinApp::GetInstance();
	mWinApp->Init();
	// DirectXCommon
	mDxCommon = DirectXCommon::GetInstance();
	mDxCommon->Init();
	
	// グローバル変数読み込み
	GlobalVariables::GetInstance()->LoadFiles();
	// メインカメラ
	MainCamera::GetInstance()->Init();
	// テクスチャマネージャ 
	TextureManager::GetInstance()->Init();
	// オブジェクト管理者クラス
	ObjectManager::GetInstance()->Init();
	// モデル管理クラス
	ModelManager::GetInstance()->Init();
	// 入力マネージャ
	InputManager::GetInstance()->Init();
	// Audio
	Audio::GetInstance()->Initialize();
	// レベルデータ読み込み
	LevelEditor::GetInstance()->CheckLevelEditorFile();
	// フレームレート
	Framerate::GetInstance()->Init();
	// パーティクルマネージャの初期化
	ParticleManager::GetInstance()->Init();
	// ポストエフェクト
	PostEffect::GetInstance()->Init();
	// ImGuiの初期化
	#ifdef _DEBUG
	mImGuiManager->Init();
	#endif // _DEBUG

}

void Framework::Update(){

	// 入力処理の更新を行う
	InputManager::GetInstance()->Update();
	// グローバル変数の更新
	GlobalVariables::GetInstance()->Update();
	
	// コマ送り時に設定した経過フレームになっていない場合、
	// 更新処理をスキップする
	Framerate::GetInstance()->Update();
	Framerate::GetInstance()->DrawGui();
	if (Framerate::GetInstance()->GetIsFrameAdvance() &&
		!Framerate::GetInstance()->IsActiveFrame()) {
	}
	else {
		// シーンの更新処理を行う
		SceneManager::GetInstance()->Update();
		// ポストエフェクト
		PostEffect::GetInstance()->Update();
	}


	// 開発用UIの表示
	#ifdef _DEBUG

	// 画面上部にメインメニューバーを追加
	ImGui::BeginMainMenuBar();
	// フレームレートの表示
	//ImGui::Text("FPS : %.2f", ImGui::GetIO().Framerate);
	// レベルエディタ
	if (ImGui::Button("LoadLevelEditor")) {
		// ボタンを押すとファイルの再読み込みを行う
		LevelEditor::GetInstance()->CheckLevelEditorFile();
	}

	// ポストエフェクトのImGui
	PostEffect::GetInstance()->Debug();

	ImGui::EndMainMenuBar();
	#endif // _DEBUG

}

void Framework::Final(){

	// Comの終了処理
	CoUninitialize();

	// 入力処理マネージャ
	InputManager::GetInstance()->Final();
	// オーディオ
	Audio::GetInstance()->Finalize();
	// ポストエフェクト
	PostEffect::GetInstance()->Finalize();
	// Scene
	SceneManager::GetInstance()->Final();
	// DirectXCommon
	DirectXCommon::GetInstance()->Final();
	// WinApp
	mWinApp->Final();
}
