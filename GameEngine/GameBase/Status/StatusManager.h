#pragma once
#include "GameEngine/Editor/GlobalVariables.h"

struct Status {
	float HP; // 体力
	float STR;// 筋力
	float VIT;// 生命力
	float AGI;// 素早さ

	void ReceiveDamage(Status attacker, float power) {
		//  [(攻撃力/2) * 攻撃倍率] - [防御力/4] でダメージを計算する
		float damage = ((attacker.STR / 2.0f) * power) - (VIT / 4.0f);

		// ダメージが負の値である場合、0に修正すること
		// そのうちダメージ最低保証などがあればここらへんも修正する
		if (damage < 0.0f) { damage = 0.0f; }

		// ヒットポイントを減少させる
		HP -= damage;
	}
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

