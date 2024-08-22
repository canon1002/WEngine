#pragma once
#include "GameEngine/Editor/GlobalVariables.h"
#include "GameEngine/GameBase/Reaction/DamageReaction.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Object/Sprite/Sprite.h"

class Status {
public: // -- 公開 メンバ関数 -- //

	// 初期化
	void Init() {
		mSprite = std::make_unique<Sprite>();
		mSprite->Init();
		mSprite->SetTexture(DirectXCommon::GetInstance()->srv_->LoadTexture("white2x2.png"));
		mSprite->SetSpriteSize(Vector2(200.0f,40.0f));
		mSprite->SetColor({0.0f,0.7f,0.0f,1.0f});


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
		mSprite->SetAnchorPoint({0.5f,0.5f});
		mSprite->SetPos(DamageReaction::GetInstance()->GetScreenPos(newPos,MainCamera::GetInstance()));
		
		// スケール更新
		float hpUIScale = ((float)HP / (float)MAXHP);
		mSprite->SetScale({ hpUIScale,1.0f });

		mSprite->Update();
	}	
	void Update() {
		// 座標を設定
		mSprite->SetPos(Vector2{10.0f,30.0f});
		mSprite->Update();
	}

	// 描画
	void Draw() {
		mSprite->Draw();
	}

public: // -- 公開 メンバ変数 -- //
	int32_t HP; // 体力
	int32_t MAXHP; // 最大体力
	int32_t STR;// 筋力
	int32_t VIT;// 生命力
	int32_t AGI;// 素早さ
	std::unique_ptr<Sprite> mSprite;

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

	Status* GetPlayerStatus() { return mPlayerStatus; }
	Status* GetBossStatus() { return mBossStatus; }
	
	void ReceiveDamage(Status* attacker, float power, Status* deffence);

private:
	// ポインタ
	GlobalVariables* mGlobalVariables = nullptr;

	// プレイヤー 
	Status* mPlayerStatus;
	// ボス
	Status* mBossStatus;

};

