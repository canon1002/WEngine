#pragma once
#include "GameEngine/Object/Sprite/Sprite.h"

/// <summary>
/// 基本的なUI動作を行える抽象クラス
/// UIの振る舞いを定義する
/// </summary>
class BaseUI{

public: // -- 公開 メンバ関数 -- //

	// コンストラクタ
	BaseUI() {};
	// デストラクタ
	virtual ~BaseUI();
	// 初期化
	virtual void Init(const std::string& name);
	// 更新
	virtual void Update();
	// 描画
	virtual void Draw();
	// ImGuiの描画
	virtual void DrawGui();
	// セーブ
	virtual void Save();
	// ロード
	virtual void Load();
	
	// UIの有効/無効を切り替える
	void SetActive(bool isActive) { mIsActive = isActive; }
	// UIの有効/無効を取得する
	bool GetActive() const { return mIsActive; }

	// UI名の取得
	std::string GetName() const { return mName; }

	// 所属シーン名の設定
	void SetSceneName(const std::string& sceneName) { mSceneName = sceneName; }
	// 所属シーン名の取得
	std::string GetSceneName()const { return mSceneName; }

	// UIタイプの取得
	std::string GetType() const { return mType; }

	// スプライトのポインタを取得する
	Sprite* GetSprite() { return mSprite.get(); }


protected: // -- 限定公開 メンバ関数 -- //


	// UI名
	std::string mName;
	// 所属シーン
	std::string mSceneName;
	// UIタイプ
	std::string mType;

	// スプライトクラス
	std::unique_ptr<Sprite> mSprite;
	// UIの有効フラグ
	bool mIsActive;

};

