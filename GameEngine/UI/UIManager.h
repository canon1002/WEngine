#pragma once

#include "GameEngine/UI/AbstractUIFactry.h"
#include <vector>

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
	void CreateUI(const std::string& name);
	// UIを削除する
	void DeleteUI(const std::string& name);
	// UIを全て削除する
	void DeleteAllUI();
	// UIを取得する


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

	// UIのリスト
	std::vector<std::unique_ptr<BaseUI>> mUIList;


};

