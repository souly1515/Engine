#pragma once

#include <memory>
#include "EntityManager.h"
#include "ComponentManager.h"
#include "Archetype.h"
#include "System.h"

namespace Engine
{
	class EngineManager
	{
		EntityManager::EntityManager EntMan;
		Component::ComponentManager CompMan;
		System::details::SystemManager SysMan;
	public:
		template<typename T_COMPONENT>
		void RegisterComponent(void) noexcept
		{
			CompMan.RegisterComponent<T_COMPONENT>();
		}

		template<typename System>
		void RegisterSystem()
		{
			SysMan.RegisterSystem<System>();
		}

		Entity CloneEntity(Entity entity);

		void RunSystems();


		template<typename COMPONENT>
		void AddComponent(Entity entity)
		{
			// get appropriate archetype
			// add entity to that archetype by copying the data over
		}

		template<typename... COMPONENTS>
		Entity CreateEntity()
		{
			return EntMan.AddEntity<COMPONENTS...>();
		}

		template<typename COMPONENT>
		COMPONENT& GetComponent(Entity entity)
		{
			return EntMan.GetComponent<COMPONENT>(entity);;
		}
	};
}
