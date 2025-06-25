#pragma once

#include "GameEngine/UI/AbstractUIFactry.h"
#include <unordered_map>

// シーン名 前方宣言
enum class SceneName;

/// <summary>
/// UIの表示・管理を行うマネージャークラス
/// </summary>
class UIManager
{
public: // -- 公開 メンバ関数 -- //
	
	// コンストラクタ
	UIManager() = default;
	// デストラクタ
	~UIManager() = default;

	// インスタンスを取得する
	static UIManager* GetInstance();
	// 終了処理
	void Final();

	// 初期化処理
	void Init();
	// 更新処理
	void Update();
	// 描画処理
	void Draw();

	// UIをJSON情報に基づいて生成する
	void CreateUI(const std::string& name,SceneName sceneName) ;
	// UIを削除する
	void DeleteUI(const std::string& name);
	// 指定シーンのUIを削除する
	void DeleteSceneUI(SceneName sceneName);
	// UIを全て削除する
	void DeleteAllUI();
	// UIを取得する
	BaseUI* GetUIPtr(const std::string name);

	// UI工場のポインタをセットする
	void SetFactory(AbstractUIFactory* factory) {
		mUIFactory = factory;
	}

private: // -- 非公開 メンバ関数 -- //

	// コピーコンストラクタと演算子オーバーロードの禁止
	UIManager(const UIManager& obj) = delete;
	UIManager& operator=(const UIManager& obj) = delete;

private: // -- 非公開 メンバ変数 -- //

	// インスタンス
	static std::unique_ptr<UIManager> instance;
	
	// UI工場のポインタ(解放禁止)
	AbstractUIFactory* mUIFactory = nullptr;
	
	// UIリスト
	// キー : 所属シーン名
	// 値 : UIポインタ配列
	std::unordered_map<SceneName, std::vector<std::unique_ptr<BaseUI>>> mUIMap;

};

