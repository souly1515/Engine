#include "EntityManager.h"

using namespace Engine::EntityManager;
using namespace Engine;

size_t EntityDB::ExtractIndex(Entity entity)
{
	return entity & IndexMask;
}

size_t EntityDB::ExtractGeneration(Entity entity)
{
	return entity & GenerationMask;
}

void Engine::EntityManager::EntityDB::SetIndex(Entity& entity, uint64_t val)
{
	entity = (entity & (~IndexMask)) | (val & IndexMask);
}

void Engine::EntityManager::EntityDB::IncrementGeneration(Entity& entity)
{
	Entity temp = entity & GenerationMask;
	temp += 1ull << (GenerationBitNum - 1);
	// apply mask to temp again for guard against overflow
	entity = (entity & (~GenerationMask)) | (temp & GenerationMask);
}

Entity_details::EntityInfo& EntityDB::GetEntityInfo(Entity entity)
{
	return m_data[ExtractIndex(entity)];
}

Entity_details::EntityInfo& Engine::EntityManager::EntityDB::CreateEntity()
{
	Entity temp = headOfFree;
	auto& info = GetEntityInfo(temp);
	auto index = ExtractIndex(info.ent);
	if (index)
	{
		headOfFree = index;
	}
	else
	{
		SetIndex(headOfFree, ExtractIndex(headOfFree) + 1);
	}
	SetIndex(info.ent, ExtractIndex(temp));
	IncrementGeneration(info.ent);
	return info;
}

void Engine::EntityManager::EntityDB::DeleteEntity(Entity entity)
{
	auto& info = GetEntityInfo(entity);
	SetIndex(info.ent, ExtractIndex(headOfFree));
	headOfFree = entity;
}

Engine::EntityManager::EntityDB::EntityDB() :
	m_data{ std::make_unique<Entity_details::EntityInfo[]>(MaxEntities) }
{
	SetIndex(headOfFree, 1);
}

std::vector<std::shared_ptr<Archetype::Archetype>> Engine::EntityManager::EntityManager::Search(Tools::query query)
{
	std::vector<std::shared_ptr<Archetype::Archetype>> res;
	
	int i = 0;
	for (auto& bits : m_archetype_bits)
	{
		if (query.Compare(bits))
		{
			res.push_back(m_archetypeList[i]);
		}
		++i;
	}

	return res;
}

Entity Engine::EntityManager::EntityManager::CloneEntity(Entity entity)
{
	return Entity();
}
