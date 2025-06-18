#pragma once
#include "GameEngine/UI/BaseUI.h"

// フェードの振る舞い
enum FadeStyle {
	FadeIn,		// フェードイン
	FadeOut,	// フェードアウト
	Loop,	// フェードイン・フェードアウトを繰り返す
};

/// <summary>
/// フェードUIクラス
/// 主に透明度の増減処理が付随するUIクラス
/// </summary>
class FadeUI :
    public BaseUI{
public: // -- 公開 メンバ関数 -- //

	// コンストラクタ
	FadeUI() = default;
	// デストラクタ
	~FadeUI() = default;
	// 初期化
	void Init(const std::string& name) override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	// ImGuiの描画
	void DrawGui() override;
	// セーブ
	void Save() override;
	// ロード
	void Load() override;

	/// <summary>
	/// フェードの発生処理
	/// </summary>
	/// <param name="style"></param>
	/// <param name="duration"></param>
	void StartFade(FadeStyle style, float duration = 1.0f);

	
	// フェード進行度の取得(割合)
	float GetFadeProgress() const {return mElapsedTime / mFadeDuration;}
	// フェード進行度の取得(=経過時間)
	float GetFadeValue() const { return mElapsedTime; }
	// フェードの有効フラグを取得
	bool GetIsFadeActive() const { return mIsFadeActive; }

	// フェードの振る舞いを取得
	FadeStyle GetFadeStyle() const { return mStyle; }

private: // -- 非公開 メンバ変数 -- //

	// フェードの時間
	float mFadeDuration;
	// フェードの経過時間
	float mElapsedTime;
	// フェードの振る舞い
	FadeStyle mStyle;
	// フェードループ時の上昇フラグ
	bool mIsFadeIn;
	// フェードの有効フラグ
	bool mIsFadeActive;

};

