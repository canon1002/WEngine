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
	mPlayerStatus->HP = (mGlobalVariables->GetFloatValue("PlayerData", "HP"));
	mPlayerStatus->STR = (mGlobalVariables->GetFloatValue("PlayerData", "STR"));
	mPlayerStatus->VIT = (mGlobalVariables->GetFloatValue("PlayerData", "VIT"));
	mPlayerStatus->AGI = (mGlobalVariables->GetFloatValue("PlayerData", "AGI"));

	mGlobalVariables->CreateGroup("BossData");
	mGlobalVariables->AddItem("BossData", "HP", mBossStatus->HP);
	mGlobalVariables->AddItem("BossData", "STR", mBossStatus->STR);
	mGlobalVariables->AddItem("BossData", "VIT", mBossStatus->VIT);
	mGlobalVariables->AddItem("BossData", "AGI", mBossStatus->AGI);

	// 登録済みのデータから数値を取得
	mBossStatus->HP = (mGlobalVariables->GetFloatValue("BossData", "HP"));
	mBossStatus->STR = (mGlobalVariables->GetFloatValue("BossData", "STR"));
	mBossStatus->VIT = (mGlobalVariables->GetFloatValue("BossData", "VIT"));
	mBossStatus->AGI = (mGlobalVariables->GetFloatValue("BossData", "AGI"));
	
	//mGlobalVariables->SaveFile("PlayerData");
	//mGlobalVariables->SaveFile("BossData");

}

void StatusManager::Update()
{

	//ImGui::Begin("StatusManager");
	//if (ImGui::TreeNode("Player")) {
	//	ImGui::DragFloat("HP", &mPlayerStatus->.HP, 1.0f, 0.0, 100.0f);
	//	ImGui::DragFloat("STR", &mPlayerStatus->.STR, 1.0f, 0.0, 100.0f);
	//	ImGui::DragFloat("VIT", &mPlayerStatus->.VIT, 1.0f, 0.0, 100.0f);
	//	ImGui::DragFloat("AGI", &mPlayerStatus->.AGI, 1.0f, 0.0, 100.0f);
	//	ImGui::TreePop();
	//}
	//if (ImGui::TreeNode("Boss")) {
	//	ImGui::DragFloat("HP", &mBossStatus->HP, 1.0f, 0.0, 100.0f);
	//	ImGui::DragFloat("STR", &mBossStatus->STR, 1.0f, 0.0, 100.0f);
	//	ImGui::DragFloat("VIT", &mBossStatus->VIT, 1.0f, 0.0, 100.0f);
	//	ImGui::DragFloat("AGI", &mBossStatus->AGI, 1.0f, 0.0, 100.0f);
	//	ImGui::TreePop();
	//}
	//ImGui::End();

}

void StatusManager::ReceiveDamage(Status* attacker,float power, Status* deffence)
{
	//  [(攻撃力/2) * 攻撃倍率] - [防御力/4] でダメージを計算する
	float damage = ((attacker->STR / 2.0f) * power) - (deffence->VIT / 4.0f);

	// ダメージが負の値である場合、0に修正すること
	// そのうちダメージ最低保証などがあればここらへんも修正する
	if (damage < 0.0f) { damage = 0.0f; }

	// ヒットポイントを減少させる
	deffence->HP -= damage;

}
