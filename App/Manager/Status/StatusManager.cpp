#include "StatusManager.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"

void StatusManager::Init()
{
	// インスタンス取得
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	mStatusList["Player"] = std::make_unique<Status>();
	mStatusList["Player"]->Init();
	
	mStatusList["Boss"] = std::make_unique<Status>();
	mStatusList["Boss"]->Init();

	globalVariables->CreateGroup("PlayerData");
	globalVariables->AddItem("PlayerData", "HP", mStatusList["Player"]->HP);
	globalVariables->AddItem("PlayerData", "STR", mStatusList["Player"]->STR);
	globalVariables->AddItem("PlayerData", "VIT", mStatusList["Player"]->VIT);
	globalVariables->AddItem("PlayerData", "AGI", mStatusList["Player"]->AGI);

	// 登録済みのデータから数値を取得
	mStatusList["Player"]->HP = int32_t(globalVariables->GetIntValue("PlayerData", "HP"));
	mStatusList["Player"]->MAXHP = int32_t(globalVariables->GetIntValue("PlayerData", "HP"));
	mStatusList["Player"]->STR = int32_t(globalVariables->GetIntValue("PlayerData", "STR"));
	mStatusList["Player"]->VIT = int32_t(globalVariables->GetIntValue("PlayerData", "VIT"));
	mStatusList["Player"]->AGI = int32_t(globalVariables->GetIntValue("PlayerData", "AGI"));

	globalVariables->CreateGroup("BossData");
	globalVariables->AddItem("BossData", "HP", mStatusList["Boss"]->HP);
	globalVariables->AddItem("BossData", "STR", mStatusList["Boss"]->STR);
	globalVariables->AddItem("BossData", "VIT", mStatusList["Boss"]->VIT);
	globalVariables->AddItem("BossData", "AGI", mStatusList["Boss"]->AGI);

	// 登録済みのデータから数値を取得
	mStatusList["Boss"]->HP = int32_t(globalVariables->GetIntValue("BossData", "HP"));
	mStatusList["Boss"]->MAXHP = int32_t(globalVariables->GetIntValue("BossData", "HP"));
	mStatusList["Boss"]->STR = int32_t(globalVariables->GetIntValue("BossData", "STR"));
	mStatusList["Boss"]->VIT = int32_t(globalVariables->GetIntValue("BossData", "VIT"));
	mStatusList["Boss"]->AGI = int32_t(globalVariables->GetIntValue("BossData", "AGI"));

}

void StatusManager::Update(){




}

void StatusManager::ReceiveDamage(const std::string& offence, const std::string& deffence){

	//  [(攻撃力/2)] - [防御力/4] でダメージを計算する
	int32_t damage = int32_t((mStatusList[offence]->STR / 2.0f)) - int32_t(mStatusList[deffence]->VIT / 4.0f);

	// ダメージが負の値である場合、0に修正すること
	// そのうちダメージ最低保証などがあればここらへんも修正する
	if (damage < 0.0f) { damage = 0; }

	// ヒットポイントを減少させる
	mStatusList[deffence]->HP -= damage;

}

void StatusManager::ReceiveDamage(const std::string& offence, const std::string& deffence, float power){
	
	//  [(攻撃力/2) * 攻撃倍率] - [防御力/4] でダメージを計算する
	int32_t damage = int32_t((mStatusList[offence]->STR / 2.0f) * power) - int32_t(mStatusList[deffence]->VIT / 4.0f);

	// ダメージが負の値である場合、0に修正すること
	// そのうちダメージ最低保証などがあればここらへんも修正する
	if (damage < 0.0f) { damage = 0; }

	// ヒットポイントを減少させる
	mStatusList[deffence]->HP -= damage;

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
