#pragma once
#include "GameEngine/Scene/BaseScene.h"
#include "GameEngine/Scene/AbstractSceneFactory.h"
#include "GameEngine/UI/AbstractUIFactry.h"

class SceneManager
{
public: // -- 公開 メンバ関数 -- //

	// インスタンス生成
	static SceneManager* GetInstance();

	// デストラクタ
	~SceneManager();

	// 初期化
	void Init();
	void Update();
	void Draw();
	void Final();

	/// <summary>
	/// シーン工場の設定
	/// </summary>
	/// <param name="sceneFactory">シーン工場(具象)</param>
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) {
		mSceneFactory = sceneFactory;
	}

	/// <summary>
	/// 次シーン予約
	/// </summary>
	/// <param name="sceneName"> シーン名 </param>
	void ChangeScene(const std::string& sceneName);

	// 外部からメニュー表示リクエストを送る
	void MenuRequest() { mMenuRequest = true; }
	// 外部からメニュー非表示リクエストを送る
	void MenuCancelRequest() { mMenuRequest = false; }

	// 外部から終了リクエストを送る
	void EndRequest() { mEndRequest = true; }

	SceneName GetCurrentSceneName();

private: // -- 非公開 メンバ関数 -- //

	// コンストラクタ
	SceneManager() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	SceneManager(const SceneManager& obj) = delete;
	SceneManager& operator=(const SceneManager& obj) = delete;

public: // -- 公開 メンバ変数 -- //

	// 終了リクエスト
	bool mEndRequest;

	// メニューリクエスト
	bool mMenuRequest;

private: // -- 非公開 メンバ変数 -- //

	// 実行中のシーン
	std::unique_ptr<BaseScene> mCurrentScene = nullptr;
	// 次のシーン
	std::unique_ptr<BaseScene> mNextScene = nullptr;
	// メニューシーン(現行シーンの上のレイヤーのようなもの)
	std::unique_ptr<BaseScene> mMenuScene = nullptr;
	// シーン工場のポインタ(解放禁止)
	AbstractSceneFactory* mSceneFactory;
	

	// UI工場
	std::unique_ptr<AbstractUIFactory> mUIFactory;



	// インスタンス
	static SceneManager* mInstance;

};

