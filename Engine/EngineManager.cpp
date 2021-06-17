#include "EngineManager.h"


Entity Engine::EngineManager::CloneEntity(Entity entity)
{
	return EntMan.CloneEntity(entity);
}


void Engine::EngineManager::RunSystems()
{
	SysMan.Run(EntMan);
}