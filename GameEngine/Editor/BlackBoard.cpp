#include "BlackBoard.h"
#include "App/Manager/GameManager.h"
#include "GameEngine/Editor/Framerate.h"

Framerate* BlackBoard::GetFramerate(){
	return Framerate::GetInstance();
}

GlobalVariables* BlackBoard::GetGlobalVariables(){
	return GlobalVariables::GetInstance();
}


float BlackBoard::CombertGameFPS(float speed)
{
	// 引数 * ゲーム全体速度 / フレームレート 
	return speed * GetFramerate()->GetBattleSpeed() / GetFramerate()->GetFramerate();
}

float BlackBoard::GetGameFPS()
{
	// ゲーム全体速度 / フレームレート 
	return GetFramerate()->GetBattleSpeed() / GetFramerate()->GetFramerate();
}

float BlackBoard::CombertBattleFPS(float speed)
{
	// 引数 * (ゲーム全体速度 *バトル速度) / フレームレート 
	return (speed * GetFramerate()->GetBattleSpeed()) / GetFramerate()->GetFramerate();
}

float BlackBoard::GetBattleFPS()
{
	// (ゲーム全体速度 *バトル速度) / フレームレート 
	return (GetFramerate()->GetBattleSpeed()) / GetFramerate()->GetFramerate();
}

