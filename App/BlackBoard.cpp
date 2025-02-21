#include "BlackBoard.h"
#include "App/Status/StatusManager.h"
#include "GameEngine/GameMaster/Framerate.h"

StatusManager* BlackBoard::GetStatusManager(){
	return StatusManager::GetInstance();
}

Framerate* BlackBoard::GetFramerate(){
	return Framerate::GetInstance();
}

Framerate* BlackBoard::GetGameSpeed()
{
	return nullptr;
}
