#pragma once
#include "GameEngine/UI/BaseUI.h"

/// <summary>
/// HPゲージなど、ゲージ系のUIを管理するクラスです。
/// ゲージの内容としては、現在値、最大値、差分値、割合などを表示します。
/// ゲージの種類としては、HPゲージ、スタミナゲージなどがあります。
/// </summary>
class GageUI :
    public BaseUI
{
public: // -- 公開 メンバ関数 -- //

	// コンストラクタ
	GageUI() = default;
	// デストラクタ
	~GageUI() = default;
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

	// ゲージの現在値を設定
	void SetGageValue(const float& value);

private: // -- 非公開 メンバ変数 -- //

	// ゲージ現在値スプライト
	std::unique_ptr<Sprite> mCurrentGageSprite; 
	// ゲージ最大値スプライト
	std::unique_ptr<Sprite> mMaxGageSprite;
	// ゲージ差分値スプライト
	std::unique_ptr<Sprite> mDiffGageSprite;

	// (ToDo)現在値テキスト
	
	// (ToDo)最大値テキスト

	// 差分値減少時のアニメーション時間
	float mDiffGageDecreaseTime = 0.0f;
	// 差分減少までの猶予時間
	float mGageDecreaseDelay = 0.0f;
	float mGageDecreaseDelayValue = 0.0f;


};

