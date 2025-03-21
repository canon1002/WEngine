#include "StatusManager.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

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
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	mPlayerStatus = std::make_shared<Status>();
	mPlayerStatus->Init();
	
	mBossStatus = std::make_shared<Status>();
	mBossStatus->Init();

	globalVariables->CreateGroup("PlayerData");
	globalVariables->AddItem("PlayerData", "HP", mPlayerStatus->HP);
	globalVariables->AddItem("PlayerData", "STR", mPlayerStatus->STR);
	globalVariables->AddItem("PlayerData", "VIT", mPlayerStatus->VIT);
	globalVariables->AddItem("PlayerData", "AGI", mPlayerStatus->AGI);

	// 登録済みのデータから数値を取得
	mPlayerStatus->HP = int32_t(globalVariables->GetIntValue("PlayerData", "HP"));
	mPlayerStatus->MAXHP = int32_t(globalVariables->GetIntValue("PlayerData", "HP"));
	mPlayerStatus->STR = int32_t(globalVariables->GetIntValue("PlayerData", "STR"));
	mPlayerStatus->VIT = int32_t(globalVariables->GetIntValue("PlayerData", "VIT"));
	mPlayerStatus->AGI = int32_t(globalVariables->GetIntValue("PlayerData", "AGI"));

	globalVariables->CreateGroup("BossData");
	globalVariables->AddItem("BossData", "HP", mBossStatus->HP);
	globalVariables->AddItem("BossData", "STR", mBossStatus->STR);
	globalVariables->AddItem("BossData", "VIT", mBossStatus->VIT);
	globalVariables->AddItem("BossData", "AGI", mBossStatus->AGI);

	// 登録済みのデータから数値を取得
	mBossStatus->HP = int32_t(globalVariables->GetIntValue("BossData", "HP"));
	mBossStatus->MAXHP = int32_t(globalVariables->GetIntValue("BossData", "HP"));
	mBossStatus->STR = int32_t(globalVariables->GetIntValue("BossData", "STR"));
	mBossStatus->VIT = int32_t(globalVariables->GetIntValue("BossData", "VIT"));
	mBossStatus->AGI = int32_t(globalVariables->GetIntValue("BossData", "AGI"));
	
}

void StatusManager::Update(){




}

void StatusManager::ReceiveDamage(std::shared_ptr<Status> attacker,float power, std::shared_ptr<Status> deffence)
{
	//  [(攻撃力/2) * 攻撃倍率] - [防御力/4] でダメージを計算する
	int32_t damage = int32_t((attacker->STR / 2.0f) * power) - int32_t(deffence->VIT / 4.0f);

	// ダメージが負の値である場合、0に修正すること
	// そのうちダメージ最低保証などがあればここらへんも修正する
	if (damage < 0.0f) { damage = 0; }

	// ヒットポイントを減少させる
	deffence->HP -= damage;

}

void StatusManager::LoadStatus(const std::string& name){

	// インスタンス取得
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	// 新規にステータス構造体を作成
	std::unique_ptr<Status> newStatus = std::make_unique<Status>();
	newStatus->Init();

	// Jsonから値を引っ張ってくる用の文字列を作成
	std::string jsonName = name + "Data";

	// Jsonから数値を取得
	newStatus->HP = globalVariables->GetIntValue(jsonName, "HP");
	newStatus->MAXHP = globalVariables->GetIntValue(jsonName, "HP");
	newStatus->STR = globalVariables->GetIntValue(jsonName, "STR");
	newStatus->VIT = globalVariables->GetIntValue(jsonName, "VIT");
	newStatus->AGI = globalVariables->GetIntValue(jsonName, "AGI");
	
	// リストに追加(所有権を渡す)
	mStatusList[name] = std::move(newStatus);

}
