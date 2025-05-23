#pragma once
#include "GameEngine/Object/Sprite/Sprite.h"

/// <summary>
/// 基本的なUI動作を行える抽象クラス
/// UIの振る舞いを定義する
/// </summary>
class BaseUI{

public: // -- 公開 メンバ関数 -- //

	// コンストラクタ
	BaseUI() = default;
	// デストラクタ
	virtual ~BaseUI() = default;
	// 終了処理
	virtual void Final();
	// 初期化
	virtual void Init(const std::string& name);
	// 更新
	virtual void Update();
	// 描画
	virtual void Draw();
	
	// UIの有効/無効を切り替える
	void SetActive(bool isActive) { mIsActive = isActive; }

	// UIの有効/無効を取得する
	bool GetActive() const { return mIsActive; }

	// スプライトのポインタを取得する
	Sprite* GetSprite() { return mSprite.get(); }


private: // -- 非公開 メンバ関数 -- //

	// UI名
	std::string mName;
	// スプライトクラス
	std::unique_ptr<Sprite> mSprite;
	// UIの有効フラグ
	bool mIsActive;

};

