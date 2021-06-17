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
				return  Component::ComponentBitset(Component::bitOffset_v<COMPONENT1>) + BitsetExpansion<COMPONENT_REST...>();
			else
				return Component::ComponentBitset(Component::bitOffset_v<COMPONENT1>);
		}

	}
	namespace EntityManager
	{
		// increase this number if more entities is needed
		static size_t constexpr IndexBitNum = 18;
		static size_t constexpr GenerationBitNum = sizeof(Entity) * 8 - IndexBitNum;
		static Entity constexpr IndexMask = ((Entity)1 << IndexBitNum) - 1;
		static Entity constexpr GenerationMask = (((Entity)1 << GenerationBitNum) - 1) ^ IndexMask;
		static Entity constexpr MaxEntities = 1ull << IndexBitNum;

		struct EntityDB
		{
			std::unique_ptr<Entity_details::EntityInfo[]> m_data;
			// reserve 0 as invalid entity
			Entity headOfFree = 0;

			size_t ExtractIndex(Entity entity);
			size_t ExtractGeneration(Entity entity);
			void SetIndex(Entity& entity, uint64_t val);
			void IncrementGeneration(Entity& entity);
			Entity_details::EntityInfo& GetEntityInfo(Entity entity);
			Entity_details::EntityInfo& CreateEntity();
			void DeleteEntity(Entity entity);
			EntityDB();
		};

		class EntityManager
		{
			EntityDB m_dataBase;

			std::shared_ptr<Archetype::Archetype_Impl<void>> m_emptyArchetype;
			std::deque<std::shared_ptr<Archetype::Archetype>>  m_archetypeList;
			std::deque<Component::ComponentBitset > m_archetype_bits;



		public:
			template<typename... COMPONENTS>
			std::shared_ptr<Archetype::Archetype> Search()
			{
				auto bits = helper::BitsetExpansion<COMPONENTS...>();
				return Search(bits);
			}

			std::vector<std::shared_ptr<Archetype::Archetype>> Search(Tools::query query);

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
				return info.ent;
			}

			template<typename Component>
			Component& GetComponent(Entity entity)
			{
				auto entInfo = m_dataBase.GetEntityInfo(entity);
				//entInfo.archetype
				return std::dynamic_pointer_cast<Archetype::Archetype_Intermediate<Component>>(
					entInfo.archetype)->GetComponent(entInfo.index);
			}

			template<typename Component>
			void AddComponent(Entity entity)
			{

			};

			Entity CloneEntity(Entity entity);
		};
	}

}