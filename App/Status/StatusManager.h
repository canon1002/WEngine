#pragma once
#include "GameEngine/Editor/GlobalVariables.h"
#include "App/Reaction/DamageReaction.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Object/Sprite/Sprite.h"

class Status {
public: // -- 公開 メンバ関数 -- //

	// 初期化
	void Init() {

		mSprites.resize(3);

		// スプライトの初期化
		for (auto& sprite : mSprites) {
			sprite = std::make_unique<Sprite>();
			sprite->Init();
			sprite->SetColor({ 0.0f,0.7f,0.0f,1.0f });
		}
		mSprites[0]->SetTexture(DirectXCommon::GetInstance()->mSrv->LoadTexture("hpBarBase.png"));
		mSprites[1]->SetTexture(DirectXCommon::GetInstance()->mSrv->LoadTexture("hpBarMidle.png"));
		mSprites[2]->SetTexture(DirectXCommon::GetInstance()->mSrv->LoadTexture("hpBarTop.png"));

		mSprites[0]->SetSpriteSize(Vector2(200.0f, 40.0f));
		mSprites[1]->SetSpriteSize(Vector2(200.0f, 40.0f));
		mSprites[2]->SetSpriteSize(Vector2(200.0f, 40.0f));

		HP = 10;
		MAXHP = 10;
		STR = 10;
		VIT = 10;
		AGI = 10;
	}

	// 更新処理
	void Update(const WorldTransform* world) {
		// 座標を設定
		Vector3 newPos = world->translation;
		newPos.x -= 0.0f;
		newPos.y += 2.0f;
		for (auto& sprite : mSprites) {
			sprite->SetAnchorPoint({ 0.5f,0.5f });
			sprite->SetPos(DamageReaction::GetInstance()->GetScreenPos(newPos, MainCamera::GetInstance()));

			// スケール更新
			if (sprite == mSprites[1]) {
				float hpUIScale = ((float)HP / (float)MAXHP);
				sprite->SetScale({ hpUIScale,1.0f });
			}
			sprite->Update();
		}
	}	

	void Update() {
		for (auto& sprite : mSprites) {
			// 座標を設定
			sprite->SetPos(Vector2{ 10.0f,30.0f });
			// スケール更新
			if (sprite == mSprites[1]) {
				float hpUIScale = ((float)HP / (float)MAXHP);
				sprite->SetScale({ hpUIScale,1.0f });
			}
			// 更新
			sprite->Update();
		}
	}

	// 描画
	void Draw() {
		for (auto& sprite : mSprites) {
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

	static StatusManager* instance;
	StatusManager() = default;
	~StatusManager() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	StatusManager(const StatusManager& obj) = delete;
	StatusManager& operator=(const StatusManager& obj) = delete;

public:
	 
	static StatusManager* GetInstance();
	void Init();
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
	
	void ReceiveDamage(std::shared_ptr<Status> attacker,float power, std::shared_ptr<Status> deffence);

private:
	// ポインタ
	GlobalVariables* mGlobalVariables = nullptr;

	// プレイヤー 
	std::shared_ptr<Status> mPlayerStatus;
	// ボス
	std::shared_ptr<Status> mBossStatus;

};

