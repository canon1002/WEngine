#pragma once
#include "GameEngine/Scene/BaseScene.h"
#include "GameEngine/Scene/AbstractSceneFactory.h"

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

private: // -- 非公開 メンバ変数 -- //

	// コンストラクタ
	SceneManager() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	SceneManager(const SceneManager& obj) = delete;
	SceneManager& operator=(const SceneManager& obj) = delete;

private: // -- 非公開 メンバ変数 -- //

	// 実行中のシーン
	std::unique_ptr<BaseScene> mCurrentScene = nullptr;
	// 次のシーン
	std::unique_ptr<BaseScene> mNextScene = nullptr;
	// メニューシーン(現行シーンの上のレイヤーのようなもの)
	std::unique_ptr<BaseScene> mMenuScene = nullptr;
	// シーン工場のポインタ(解放禁止)
	AbstractSceneFactory* mSceneFactory;

	// インスタンス
	static SceneManager* mInstance;

};

