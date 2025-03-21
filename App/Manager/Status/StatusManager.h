#pragma once
#include "GameEngine/Editor/GlobalVariables.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Object/Sprite/Sprite.h"

class Status {
public: // -- 公開 メンバ関数 -- //

	// 初期化
	void Init() {

		mSprites.resize(2);

		// スプライトの初期化
		for (auto& sprite : mSprites) {
			sprite = std::make_unique<Sprite>();
			sprite->Init();
			sprite->SetColor({ 1.0f,1.0f,1.0f,1.0f });
			sprite->SetTexture(TextureManager::GetInstance()->LoadTexture("white2x2.dds"));
			sprite->SetAnchorPoint(Vector2(0.5f, 0.5f));
			sprite->SetSpriteSize(Vector2(200.0f, 20.0f));
		}
		
		// HPバーの背景部分
		mSprites[0]->SetScale(Vector2(1.05f, 1.0f));	// 横方向に大きく
		mSprites[0]->SetColor({ 0.7f,0.7f,0.7f,1.0f });	// 灰色に変更

		// HPバーの体力部分
		mSprites[1]->SetScale(Vector2(1.0f, 0.95f));	// 縦方向をやや小さく
		mSprites[1]->SetColor({0.6f,0.0f,0.0f,1.0f});	// 赤色に変更

		// ステータスの仮初期化(このあとjsonから数値をひっぱてくる)
		HP = 10;
		MAXHP = 10;
		STR = 10;
		VIT = 10;
		AGI = 10;
	}

	// 更新処理
	void Update() {
		for (auto& sprite : mSprites) {

			// スケール更新
			if (sprite == mSprites[1]) {
			
				// 残りのHPに応じてスケールを変更
				sprite->SetScale(Vector2(
					((float)HP / (float)MAXHP), 
					sprite->GetScale().y
				));
			}

			// 更新
			sprite->Update();
		}
	}

	// 描画処理 (2d座標を設定)
	void Draw(const Vector2& pos) {
		for (auto& sprite : mSprites) {
			sprite->SetPos(pos);
			sprite->Draw();
		}
	}

public: // -- 公開 メンバ変数 -- //
	int32_t HP; // 体力
	int32_t MAXHP; // 最大体力
	int32_t STR;// 筋力
	int32_t VIT;// 生命力
	int32_t AGI;// 素早さ
	std::vector<std::unique_ptr<Sprite>> mSprites;

};


class StatusManager {

private: // -- 非公開 メンバ関数 -- //

	StatusManager() = default;
	~StatusManager() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	StatusManager(const StatusManager& obj) = delete;
	StatusManager& operator=(const StatusManager& obj) = delete;

public: // -- 公開 メンバ関数 -- //
	 
	// インスタンスを取得する
	static StatusManager* GetInstance();
	
	// 初期化
	void Init();
	// 更新
	void Update();
	


	std::shared_ptr<Status> GetPlayerStatus() { return mPlayerStatus; }
	void GetPlayerStatus(Status& st) { 
		st.HP = mPlayerStatus->HP;
		st.MAXHP = mPlayerStatus->MAXHP;
		st.STR = mPlayerStatus->STR;
		st.VIT = mPlayerStatus->VIT;
		st.AGI = mPlayerStatus->AGI;
		st.mSprites = std::move(mPlayerStatus->mSprites);
	}
	std::shared_ptr<Status> GetBossStatus() { return mBossStatus; }
	
	// ダメージ計算処理
	void ReceiveDamage(std::shared_ptr<Status> attacker,float power, std::shared_ptr<Status> deffence);

	// リスト内から登録済みのステータスを取得する
	Status* GetStatus(const std::string& name) {
		return mStatusList[name].get();
	}

	// ステータス読み込み関数
	void LoadStatus(const std::string& name);

private: // -- 非公開 メンバ変数 -- //

	// インスタンス
	static StatusManager* instance;

	// 各キャラクターのステータス情報
	std::map<std::string, std::unique_ptr<Status>> mStatusList;

	// プレイヤー 
	std::shared_ptr<Status> mPlayerStatus;
	// ボス
	std::shared_ptr<Status> mBossStatus;

};

