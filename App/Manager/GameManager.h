#pragma once
#include "App/Manager/Status/StatusManager.h"
#include "App/Manager/Reaction/DamageReaction.h"
#include "GameEngine/Component/Collider/CollisionManager.h"
#include "App/Actor/Collider/GameCollider.h"
#include "App/UI/GageUI.h"

// 前方宣言
class Player;
class BossEnemy;

/// <summary>
/// ゲーム管理者クラス(演出・パラメータ等を一括管理する)
/// </summary>
class GameManager
{
private: // -- 非公開 メンバ関数 -- //

	GameManager() = default;
	~GameManager() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	GameManager(const GameManager& obj) = delete;
	GameManager& operator=(const GameManager& obj) = delete;

public: // -- 公開 メンバ関数 -- //

	// インスタンスを取得する
	static GameManager* GetInstance();
	// 終了処理
	void Final();
	// 初期化処理
	void Init();
	// 更新処理
	void Update();
	// 描画処理
	void Draw();

	// -- 初期化処理 関数群 -- //

	// UIの初期化を行う
	void InitUI();

	// -- 更新処理 関数群 -- //

	// コライダーリストの更新を行う
	void UpdateColliderList();
	// 衝突判定 ＆ 衝突後処理
	void UpdateCollisionManager();
	// ステータス並びにUIの更新処理
	void UpdateStatusManager();



	// --  衝突判定 関数群 -- //

	// コライダーをリストに追加する
	void SetCollider(std::shared_ptr<GameCollider> collider);
	// 指定した種別IDのコライダーが衝突したか調べる
	bool ChackOnCollided(uint32_t typeID);



	// -- アクセッサ 関数群 -- //


	// プレイヤークラスのポインタをセットする
	void SetPlayer(Player* player);
	// ボス敵クラスのポインタをセットする
	void SetBossEnemy(BossEnemy* boss);

	// ヒットストップのリクエストを取得する
	bool GetHitStopRequest(){
		// 戻り値用の変数を宣言
		bool request = mIsRequestHitStop;
		// もとの数値をfalseにしておく(ヒットストップによる永久停止を防ぐ)
		mIsRequestHitStop = false;
		// 保存していた値を返す
		return request;	
	}
	// ヒットストップの発生時間を取得する
	float GetHitStopDuration(){
		// 戻り値用の変数を宣言
		float duration = mHitStopDuration;
		// もとの数値を0にしておく(ヒットストップによる永久停止を防ぐ)
		mHitStopDuration = 0.0f;
		// 保存していた値を返す
		return duration;
	}

	// ステータス取得用アクセッサ
	Status* GetStatus(const std::string& name){
		return mStatusManager->GetStatus(name);
	}

	// -- その他 -- //

	// 押し出し処理
	void ResolveCollision();

private: // -- 非公開 メンバ変数 -- //

	// インスタンス
	static GameManager* instance;

	// プレイヤーキャラのポインタ
	Player* mPlayer;
	// 敵キャラのポインタ
	BossEnemy* mBoss;
	

	// -- 衝突判定関係 -- //

	// 衝突判定マネージャ
	std::unique_ptr<CollisionManager> mCollisionManager;
	// 各属性ごとのコライダーリスト
	std::unordered_map<uint32_t, std::list<std::weak_ptr<GameCollider>>> mColliderList;

	// ヒットストップリクエスト
	bool mIsRequestHitStop = false;
	// ヒットストップの発生期間
	float mHitStopDuration = 0.0f;


	// -- ステータス関係 -- //

	// ステータスマネージャ
	std::unique_ptr<StatusManager> mStatusManager;



	// -- UI -- //

	// プレイヤー - HPゲージ
	GageUI* mPlayerHpGage;
	// プレイヤー - スタミナゲージ
	GageUI* mPlayerStaminaGage;

	// 敵 - HPゲージ
	GageUI* mBossHpGage;
	// 敵 - ブレイクゲージ
	GageUI* mBossBreakGage;


};

