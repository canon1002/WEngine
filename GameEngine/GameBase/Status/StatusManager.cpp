#include "StatusManager.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/GameBase/Reaction/DamageReaction.h"

StatusManager* StatusManager::instance = nullptr;

StatusManager* StatusManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new StatusManager();
	}
	return instance;
}

void StatusManager::Init()
{
	// インスタンス取得
	mGlobalVariables = GlobalVariables::GetInstance();
	mPlayerStatus = new Status();
	mPlayerStatus->HP = 10;
	mPlayerStatus->STR = 10;
	mPlayerStatus->VIT = 10;
	mPlayerStatus->AGI = 10;
	
	mBossStatus = new Status();
	mBossStatus->HP = 10;
	mBossStatus->STR = 10;
	mBossStatus->VIT = 10;
	mBossStatus->AGI = 10;
	

	mGlobalVariables->CreateGroup("PlayerData");
	mGlobalVariables->AddItem("PlayerData", "HP", mPlayerStatus->HP);
	mGlobalVariables->AddItem("PlayerData", "STR", mPlayerStatus->STR);
	mGlobalVariables->AddItem("PlayerData", "VIT", mPlayerStatus->VIT);
	mGlobalVariables->AddItem("PlayerData", "AGI", mPlayerStatus->AGI);

	// 登録済みのデータから数値を取得
	mPlayerStatus->HP = int32_t(mGlobalVariables->GetFloatValue("PlayerData", "HP"));
	mPlayerStatus->STR = int32_t(mGlobalVariables->GetFloatValue("PlayerData", "STR"));
	mPlayerStatus->VIT = int32_t(mGlobalVariables->GetFloatValue("PlayerData", "VIT"));
	mPlayerStatus->AGI = int32_t(mGlobalVariables->GetFloatValue("PlayerData", "AGI"));

	mGlobalVariables->CreateGroup("BossData");
	mGlobalVariables->AddItem("BossData", "HP", mBossStatus->HP);
	mGlobalVariables->AddItem("BossData", "STR", mBossStatus->STR);
	mGlobalVariables->AddItem("BossData", "VIT", mBossStatus->VIT);
	mGlobalVariables->AddItem("BossData", "AGI", mBossStatus->AGI);

	// 登録済みのデータから数値を取得
	mBossStatus->HP = int32_t(mGlobalVariables->GetFloatValue("BossData", "HP"));
	mBossStatus->STR = int32_t(mGlobalVariables->GetFloatValue("BossData", "STR"));
	mBossStatus->VIT = int32_t(mGlobalVariables->GetFloatValue("BossData", "VIT"));
	mBossStatus->AGI = int32_t(mGlobalVariables->GetFloatValue("BossData", "AGI"));
	
}

void StatusManager::Update()
{

}

void StatusManager::ReceiveDamage(Status* attacker,float power, Status* deffence)
{
	//  [(攻撃力/2) * 攻撃倍率] - [防御力/4] でダメージを計算する
	int32_t damage = int32_t((attacker->STR / 2.0f) * power) - int32_t(deffence->VIT / 4.0f);

	// ダメージが負の値である場合、0に修正すること
	// そのうちダメージ最低保証などがあればここらへんも修正する
	if (damage < 0.0f) { damage = 0; }

	// ヒットポイントを減少させる
	deffence->HP -= damage;

}
