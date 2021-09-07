#include "EntityManager.h"
#include "Logger.h"
#include <algorithm>
#include <string>

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
	if constexpr (Logger::LogEnabled())
	{
		Logger* log = Logger::GetInstance();
		std::string data = "current head of free: " +
			std::to_string(headOfFree) + "\n";
		log->Log(data);
	}
	Entity temp = headOfFree;
	auto& info = GetEntityInfo(temp); // gets the entity data from the head's index
	auto index = ExtractIndex(info.ent); // gets the index to the infoList 
	// if its a valid index then it means its a dead entity
	// so reuse this entity info
	if (index)
	{
		headOfFree = info.ent;
		if constexpr (Logger::LogEnabled())
		{
			Logger* log = Logger::GetInstance();
			std::string data = "Creating Entity(orignially dead) from Archetype: " + 
				std::to_string((unsigned long)info.archetype.get()) + "\n";
			log->Log(data);
			data = "new head of free is " + std::to_string(headOfFree) + "\n";
			log->Log(data);
		}

	}
	else
	{
		SetIndex(headOfFree, headOfFree + 1);

		if constexpr (Logger::LogEnabled())
		{
			Logger* log = Logger::GetInstance();
			std::string data = "Creating Entity(new) from Archetype: " +
				std::to_string((unsigned long)info.archetype.get()) + "\n";
			log->Log(data);
		}
	}
	ResetZombie(temp);
	SetIndex(temp, temp);
	IncrementGeneration(info.ent);
	if constexpr (Logger::LogEnabled())
	{
		Logger* log = Logger::GetInstance();
		std::string data = "Final Entity Generated: " +
			std::to_string(info.ent) + "\n";
		auto t = GetEntityInfo(headOfFree);
		data += "Next Free is " + std::to_string(t.ent) + "\n";
		log->Log(data);
	}
	return info;
}

void Engine::EntityManager::EntityDB::DeleteEntity(Entity entity)
{

	auto& info = GetEntityInfo(entity);
	auto movedIdx = info.archetype->DeleteEntity(info.index);
	
	if constexpr (Logger::LogEnabled())
	{
		Logger* log = Logger::GetInstance();
		std::string data = "Deleting from Archetype: " + std::to_string((unsigned long)info.archetype.get()) + " Entity: " + std::to_string(info.ent) + " Index: " + std::to_string(info.index) + "\n";
		log->Log(data);
	}

	// not the last entity in the archetype
	if (movedIdx != info.index)
	{
		Entity movedEntity = info.archetype->GetComponent<EntityComponent>(info.index).entity;
		auto& movedInfo = GetEntityInfo(movedEntity);

		if constexpr (Logger::LogEnabled())
		{
			Logger* log = Logger::GetInstance();
			std::string data = "Swapping Deleted Entity with Entity: " + std::to_string(movedInfo.ent) + " Index: " + std::to_string(movedInfo.index) + "\n";
			log->Log(data);
		}

		movedInfo.index = info.index;
	}

	info.ent = headOfFree;
	headOfFree = entity;
	if constexpr (Logger::LogEnabled())
	{
		Logger* log = Logger::GetInstance();
		std::string data = "Setting Head of free to be: " + std::to_string(headOfFree) + "\n";
		data += "next free would be: " + std::to_string(info.ent) + "\n";
		log->Log(data);
	}
	
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
