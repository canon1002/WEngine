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
	globalVariables_ = GlobalVariables::GetInstance();

}

void StatusManager::Update()
{

}
