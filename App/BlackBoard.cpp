#include "BlackBoard.h"
#include "App/Manager/GameManager.h"
#include "GameEngine/GameMaster/Framerate.h"

StatusManager* BlackBoard::GetStatusManager(){
	return StatusManager::GetInstance();
}

Framerate* BlackBoard::GetFramerate(){
	return Framerate::GetInstance();
}

GlobalVariables* BlackBoard::GetGlobalVariables()
{
	return GlobalVariables::GetInstance();
}

float BlackBoard::CombertBattleFPS(float speed)
{
	// 引数 * フレームレート * (ゲーム全体速度 * バトル速度)
	return (speed * GetFramerate()->GetBattleSpeed()) / GetFramerate()->GetFramerate();
}

float BlackBoard::GetBattleFPS()
{
	// 引数 * フレームレート * (ゲーム全体速度 * バトル速度)
	return (GetFramerate()->GetBattleSpeed()) / GetFramerate()->GetFramerate();
}

