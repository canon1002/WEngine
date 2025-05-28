#include "MenuScene.h"
#include "GameEngine/Effect/PostEffect/PostEffect.h"
#include "GameEngine/Editor/Framerate.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
#include "GameEngine/Input/InputManager.h"

void MenuScene::Final(){

}

void MenuScene::Init() {
	
	// シーン名の設定
	mSceneName = "Menu";

	// メニュー背景
	UIManager::GetInstance()->CreateUI("MenuBackground", SceneName::Menu);
	mMenuBackground = dynamic_pointer_cast<BaseUI>(UIManager::GetInstance()->GetUIPtr("MenuBackground"));

}

void MenuScene::Update() {

	// ゲーム終了リクエスト
	// 現段階では LSHIFT + ESCAPE で終了
	// 後々ボタン式にする
	if (InputManager::GetInstance()->GetKey()->GetPushKey(DIK_LSHIFT) &&
		InputManager::GetInstance()->GetKey()->GetPushKey(DIK_ESCAPE)) {

		// 終了リクエストを送る
		SceneManager::GetInstance()->EndRequest();
	}

	// メニューシーン解除リクエスト
	// スタートボタン/ESCAPEキーでメニューを閉じる
	if (InputManager::GetInstance()->GetPused(Gamepad::Button::START) ||
		InputManager::GetInstance()->GetKey()->GetTriggerKey(DIK_ESCAPE)) {
		
		// メニューシーンを閉じるリクエストを送る 
		SceneManager::GetInstance()->MenuCancelRequest();
	}


	// (TODO)シーン切り替えリクエスト
	// タイトル・ゲームシーン・チュートリアル遷移・リトライ などを後々実装する

}

void MenuScene::Draw() {
	
}

void MenuScene::DrawUI(){
	
}
