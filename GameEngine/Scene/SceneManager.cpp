#include "SceneManager.h"
#include "GameEngine/Effect/PostEffect/PostEffect.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
#include "GameEngine/UI/UIManager.h"
#include "App/UI/UIFactory.h"
#include "Externals/magic_enum/magic_enum.hpp"

// インスタンス
SceneManager* SceneManager::mInstance = nullptr;

// インスタンス取得
SceneManager* SceneManager::GetInstance()
{
	// インスタンスがnullptrであれば生成する
	if (mInstance == nullptr) {
		mInstance = new SceneManager();
	}
	return mInstance;
}

// デストラクタ
SceneManager::~SceneManager() {
	// 最終シーンの終了処理
	mCurrentScene->Final();
	mCurrentScene.reset();

	// メニューの終了処理
	if (mMenuScene) {
		mMenuScene->Final();
		mMenuScene.reset();
	}

	// インスタンスの破棄
	delete mInstance;
	mInstance = nullptr;
}

// 初期化
void SceneManager::Init(){

	// UI工場を作成し、マネージャにセット
	mUIFactory = std::make_unique<UIFactory>();
	UIManager::GetInstance()->SetFactory(mUIFactory.get());

	// メニューシーンの生成
	mMenuScene = mSceneFactory->CreateScene("Menu");
	// メニューシーンの初期化
	mMenuScene->Init();

	// 終了リクエスト
	mEndRequest = false;
}

void SceneManager::Update(){

	// シーン切り替え
	if (mNextScene) {
		// 旧シーンの終了
		if (mCurrentScene) {
			mCurrentScene->Final();
			mCurrentScene.reset();
		}

		// シーン切り替え
		mCurrentScene = std::move(mNextScene);
		mNextScene = nullptr;

		// シーンマネージャをセット
		mCurrentScene->SetSceneManager(this);

		// 切り替えたシーンの初期化
		mCurrentScene->Init();
	}

	
	// メニューシーンの実行
	if (mMenuRequest) {

		// メニューシーンの更新
		mMenuScene->Update();
	}
	// 実行中のシーンの更新
	else {
		
		// ESCキー/スタートボタンでメニューリクエスト
		// 終了リクエストはメニューシーンで行う
		if (InputManager::GetInstance()->GetPused(Gamepad::Button::START) ||
			InputManager::GetInstance()->GetKey()->GetTriggerKey(DIK_ESCAPE)) {

			// メニューシーンの実行リクエストを送る
			MenuRequest();
		}
		
		// 実行中のシーンの更新
		mCurrentScene->Update();
	}
	
	// UIの更新
	UIManager::GetInstance()->Update();

}

void SceneManager::Draw(){

	// 描画前処理 -- RenderTexture --
	DirectXCommon::GetInstance()->PreDrawForRenderTarget();

	// 実行中のシーンの描画
	mCurrentScene->Draw();

	// メニューシーンの描画
	if (mMenuRequest && mMenuScene) {
		mMenuScene->Draw();
	}

	// 描画後処理 -- RenderTexture --
	DirectXCommon::GetInstance()->PostDrawForRenderTarget();
	
	// 描画前処理
	DirectXCommon::GetInstance()->PreDraw();

	// ポストエフェクトの描画
	PostEffect::GetInstance()->PreDraw();
	PostEffect::GetInstance()->Draw();

	// UIの描画
	mCurrentScene->DrawUI();
	// メニューシーンの描画
	if (mMenuRequest && mMenuScene) {
		mMenuScene->DrawUI();
	}
	UIManager::GetInstance()->Draw();

#ifdef _DEBUG
	// ImGuiの描画
	ImGuiManager::GetInstance()->Draw();
#endif // _DEBUG

	// 描画後処理
	DirectXCommon::GetInstance()->PostDraw();

}

void SceneManager::Final(){

}

void SceneManager::ChangeScene(const std::string& sceneName){

	// シーン工場クラスの確認(なければエラー)
	assert(mSceneFactory);
	// 次シーンが存在していたらエラー
	assert(mNextScene == nullptr);

	// 次シーンの生成
	mNextScene = mSceneFactory->CreateScene(sceneName);

}

SceneName SceneManager::GetCurrentSceneName(){

	// メニューシーンの実行リクエストを確認
	if (mMenuRequest) {

		// リクエストが確認できた場合、
		// メニューシーンを実行シーンとして返す
		return magic_enum::enum_cast<SceneName>(
			mMenuScene->GetSceneName()).value() ;
	}

	// 現在のシーン名を文字列で取得
	std::string sceneNameStr = mCurrentScene->GetSceneName();

	// 取得したシーン名を列挙型に変換
	SceneName sceneName = magic_enum::enum_cast<SceneName>(sceneNameStr).value();

	// 列挙型のシーン名を返す
	return sceneName;
}
