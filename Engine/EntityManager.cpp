#include "EntityManager.h"

using namespace Engine::EntityManager;
using namespace Engine;
using namespace Engine::EntityManager::EntityHelper;

size_t EntityHelper::ExtractIndex(Entity entity)
{
	return entity & IndexMask;
}

size_t EntityHelper::ExtractGeneration(Entity entity)
{
	return entity & GenerationMask;
}

bool Engine::EntityManager::EntityDB::IsZombie(Entity entity)
{
	return entity & ZombieMask;
}

Entity Engine::EntityManager::EntityDB::ToggleZombie(Entity entity)
{
	return m_data[ExtractIndex(entity)].ent = (entity ^ ZombieMask);
}

Entity Engine::EntityManager::EntityDB::ResetZombie(Entity entity)
{
	return m_data[ExtractIndex(entity)].ent = (entity & (~ZombieMask));
}

Entity Engine::EntityManager::EntityDB::SetZombie(Entity entity)
{
	return m_data[ExtractIndex(entity)].ent = (entity | ZombieMask);
}

void Engine::EntityManager::EntityHelper::SetIndex(Entity& entity, uint64_t val)
{
	entity = (entity & (~IndexMask)) | (val & IndexMask);
}

void Engine::EntityManager::EntityHelper::IncrementGeneration(Entity& entity)
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
	ResetZombie(info.ent);
	SetIndex(info.ent, ExtractIndex(temp));
	IncrementGeneration(info.ent);
	return info;
}

void Engine::EntityManager::EntityDB::DeleteEntity(Entity entity)
{
	auto& info = GetEntityInfo(entity);
	auto movedIdx = info.archetype->DeleteEntity(info.index);
	if (movedIdx != info.index)
	{
		Entity movedEntity = info.archetype->GetComponent<EntityComponent>(info.index).entity;
		auto& movedInfo = GetEntityInfo(movedEntity);
		movedInfo.index = info.index;
	}

	SetIndex(info.ent, ExtractIndex(headOfFree));
	headOfFree = entity;
}

Engine::EntityManager::EntityDB::EntityDB() :
	m_data{ std::make_unique<Entity_details::EntityInfo[]>(MaxEntities) }
{
	SetIndex(headOfFree, 1);
}

Engine::ArchetypeVector Engine::EntityManager::EntityManager::Search(const Tools::Query& query)
{
	std::vector<std::shared_ptr<Archetype::Archetype>> res;
	
	int i = 0;
	for (auto& bits : m_archetype_bits)
	{
		if (query.Compare(bits))
		{
			if(m_archetypeList[i]->entityNum > 0)
				res.push_back(m_archetypeList[i]);
		}
		++i;
	}

	return Engine::ArchetypeVector{ res };
}

void Engine::EntityManager::EntityManager::DeleteEntity(Entity ent)
{
	auto info = m_dataBase.GetEntityInfo(ent);
	ent = info.archetype->GetComponent<EntityComponent>(info.index).entity = m_dataBase.ToggleZombie(ent);
	m_destroyedEntities.push_back(ent);
}

void Engine::EntityManager::EntityManager::UpdateStructuralComponents()
{
	std::sort(
			m_destroyedEntities.begin(),
			m_destroyedEntities.end(),
		[&](const Entity& ent1, const Entity& ent2)
		{
			auto info1 = m_dataBase.GetEntityInfo(ent1);
			auto info2 = m_dataBase.GetEntityInfo(ent2);
			return info1.index > info2.index;
		});
	for (auto i = 0; i < m_destroyedEntities.size(); ++i)
	{
		m_dataBase.DeleteEntity(m_destroyedEntities[i]);
	}
	m_destroyedEntities.clear();
}

bool Engine::EntityManager::EntityManager::IsZombie(Entity ent)
{
	return m_dataBase.IsZombie(ent);;
}

Entity Engine::EntityManager::EntityManager::CloneEntity(Entity entity)
{
	return Entity();
}

Entity& ArchetypeVector::ArchetypeIterator::operator*()
{
	auto& entComp = (*itr)->GetComponent<EntityComponent>(index);
	return entComp.entity;
}

ArchetypeVector::ArchetypeIterator& ArchetypeVector::ArchetypeIterator::operator++()
{
	++index;
	if (index >= (*itr)->entityNum)
	{
		++itr;
		index = 0;
	}
	return *this;
}

bool ArchetypeVector::ArchetypeIterator::operator==(const ArchetypeIterator& rhs)
{
	return (itr == rhs.itr) && (index == rhs.index);
}

bool ArchetypeVector::ArchetypeIterator::operator!=(const ArchetypeIterator& rhs)
{
	return !(*this == rhs);
}

Engine::ArchetypeVector::ArchetypeIterator::ArchetypeIterator(const ArchetypeIterator& rhs)
	:
	itr{ rhs.itr },
	index{ rhs.index }
{
}

Engine::ArchetypeVector::ArchetypeIterator::ArchetypeIterator(ArchetypeVector::StoreType::iterator init):
	itr{ init },
	index{ 0 }
{
}

ArchetypeVector::ArchetypeIterator ArchetypeVector::begin()
{
	return ArchetypeIterator(m_store.begin());
}

ArchetypeVector::ArchetypeIterator ArchetypeVector::end()
{
	return ArchetypeIterator(m_store.end());
}

Engine::ArchetypeVector::ArchetypeVector(StoreType& store):
	m_store{ store }
{
}

Engine::ArchetypeVector::StoreType& Engine::ArchetypeVector::GetStore()
{
	return m_store;
}
