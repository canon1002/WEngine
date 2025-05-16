#include "SceneManager.h"
#include "GameEngine/Effect/PostEffect/PostEffect.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"

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

	// TODO:メニューシーンとの切り替え処理

	//// メニューシーンの実行
	//if (mMenuScene->IsActive()) {
	//
	//}
	//// 実行中のシーンの更新
	//else {
	//	
	//}
	mCurrentScene->Update();
}

void SceneManager::Draw(){

	// 描画前処理 -- RenderTexture --
	DirectXCommon::GetInstance()->PreDrawForRenderTarget();

	// 実行中のシーンの描画
	mCurrentScene->Draw();

	// 描画後処理 -- RenderTexture --
	DirectXCommon::GetInstance()->PostDrawForRenderTarget();
	
	// 描画前処理
	DirectXCommon::GetInstance()->PreDraw();

	// ポストエフェクトの描画
	PostEffect::GetInstance()->PreDraw();
	PostEffect::GetInstance()->Draw();

	// UIの描画
	mCurrentScene->DrawUI();

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
