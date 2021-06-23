#pragma once
#include "Archetype.h"
#include "EntityHelper.h"
#include "Entity.h"
#include "Bitset.h"
#include "ComponentManager.h"
#include "Query.h"
#include <deque>
#include <memory>
#include <vector>

namespace Engine
{
	namespace helper
	{
		template <typename COMPONENT1, typename ...COMPONENT_REST>
		Component::ComponentBitset BitsetExpansion()
		{
			if constexpr (sizeof...(COMPONENT_REST) > 0)
			{
				auto temp = Component::component_info_v<COMPONENT1>.m_UID;
				return  Component::ComponentBitset(temp) + BitsetExpansion<COMPONENT_REST...>();
			}
			else
				return Component::ComponentBitset(Component::bitOffset_v<COMPONENT1>);
		}

	}

	class ArchetypeVector
	{
		using StoreType = std::vector<std::shared_ptr<Archetype::Archetype>>;
		StoreType m_store;

	public:
		class ArchetypeIterator
		{
			StoreType::iterator itr;
			Archetype::ChunkIndex index = 0;
		public:
			Entity& operator*();

			ArchetypeIterator& operator++();
			bool operator==(const ArchetypeIterator& rhs);
			bool operator!=(const ArchetypeIterator& rhs);

//============= Constructors =====================
			ArchetypeIterator(const ArchetypeIterator& rhs);
			ArchetypeIterator(StoreType::iterator init);

		};
		
		ArchetypeIterator begin();
		ArchetypeIterator end();
		ArchetypeVector(StoreType& store);
		StoreType& GetStore();
	};


	namespace EntityManager
	{
		// increase this number if more entities is needed
		static size_t constexpr IndexBitNum = 18;
		static size_t constexpr GenerationBitNum = sizeof(Entity) * 8 - IndexBitNum - 1;
		static Entity constexpr IndexMask = ((Entity)1 << IndexBitNum) - 1;
		static Entity constexpr ZombieMask = ((Entity)1 << IndexBitNum);
		static Entity constexpr GenerationMask = (-1) ^ IndexMask ^ ZombieMask;
		static Entity constexpr MaxEntities = 1ull << IndexBitNum;

		namespace EntityHelper
		{
			size_t ExtractIndex(Entity entity);
			size_t ExtractGeneration(Entity entity);
			void SetIndex(Entity& entity, uint64_t val);
			void IncrementGeneration(Entity& entity);
		}

		struct EntityDB
		{
			std::unique_ptr<Entity_details::EntityInfo[]> m_data;
			// reserve 0 as invalid entity
			Entity headOfFree = 0;

			bool IsZombie(Entity entity);
			Entity ToggleZombie(Entity entity);
			Entity ResetZombie(Entity entity);
			Entity SetZombie(Entity entity);
			Entity_details::EntityInfo& GetEntityInfo(Entity entity);
			Entity_details::EntityInfo& CreateEntity();
			void DeleteEntity(Entity entity);
			EntityDB();
		};

		class EntityManager
		{
			EntityDB m_dataBase;

			std::shared_ptr<Archetype::Archetype_Impl<>> m_emptyArchetype;
			std::deque<std::shared_ptr<Archetype::Archetype>>  m_archetypeList;
			std::deque<Component::ComponentBitset > m_archetype_bits;

			std::vector<Entity> m_destroyedEntities;
		public:
			template<typename... COMPONENTS>
			std::shared_ptr<Archetype::Archetype> Search()
			{
				auto bits = helper::BitsetExpansion<COMPONENTS...>();
				return Search(bits);
			}

			ArchetypeVector Search(const Tools::Query& query);

			template<>
			std::shared_ptr<Archetype::Archetype> Search<>()
			{
				return m_emptyArchetype;
			}

			std::shared_ptr<Archetype::Archetype> Search(Component::ComponentBitset bits)
			{
				int i = 0;
				for (auto& archBit : m_archetype_bits)
				{
					if (archBit == bits)
						return m_archetypeList[i];
					++i;
				}
				return {};
			}

			template<typename... COMPONENTS>
			Entity AddEntity()
			{
				auto dyn = std::dynamic_pointer_cast<Archetype::Archetype_Impl<COMPONENTS...>>(Search<COMPONENTS...>());
				// search for the appropriate archetype and add an entity to it
				if (!dyn)
				{
					dyn = std::make_shared<Archetype::Archetype_Impl<COMPONENTS...>>();
					auto bits = helper::BitsetExpansion<COMPONENTS...>();

					m_archetypeList.push_back(dyn);
					m_archetype_bits.push_back(bits);
				}
				Archetype::ChunkIndex index = dyn->AddEntity();
				auto& info = m_dataBase.CreateEntity();
				info.archetype = dyn;
				info.index = index;
				dyn->GetComponent<EntityComponent>(index).entity = info.ent;
				return info.ent;
			}

			void RemoveEntity(Entity ent);

			void UpdateStructuralComponents();

			template<typename Component>
			std::decay_t<Component>& GetComponent(Entity entity)
			{
				auto entInfo = m_dataBase.GetEntityInfo(entity);
				//entInfo.archetype
				return entInfo.archetype->GetComponent<std::decay_t<Component>>(entInfo.index);
			}
			template<typename Component>
			std::decay_t<Component>* TryGetComponent(Entity entity)
			{
				auto entInfo = m_dataBase.GetEntityInfo(entity);
				//entInfo.archetype
				return entInfo.archetype->GetComponent<std::decay_t<Component>*>(entInfo.index);
			}

			bool IsZombie(Entity ent);

			template<typename Component>
			void AddComponent(Entity entity)
			{

			};

			Entity CloneEntity(Entity entity);
		};
	}

}