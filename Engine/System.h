#pragma once
#include <vector>
#include <memory>

#include "EntityManager.h"
#include "Bitset.h"


namespace Engine
{

	namespace System
	{
    // user systems has to inherit from this to use execute
    struct SystemBase
    {
      void Execute(EntityManager::EntityManager&) {};
    };
    namespace details
    {

      template< typename user_system >
      struct CompletedSystem final : user_system, SystemBase
      {
        using func_traits = xcore::function::traits<user_system>;
        Tools::query m_Query;

        void Run(EntityManager::EntityManager& GM) noexcept override
        {
          constexpr bool has_Execute = requires(const user_system & us) {
            user_system::Execute;
          };

          if constexpr (has_Execute && &user_system::Execute != &SystemBase::Execute)
          {
            user_system::Execute(GM);
          }
          else
          {
            // generate query
            auto archetypes = GM.Search(m_Query);

            // maybe have a fore each function that takes archetypes and a functor
            for (auto& archetype : archetypes)
            {
              archetype->RunWithFunctor(*this);
            }
          }
        }
      };

      struct SystemManager
      {
      private:
        struct info
        {
          using call_run = void(SystemBase&, EntityManager::EntityManager& GM);
          std::unique_ptr<SystemBase> m_sys;
          call_run* m_callRun;
        };

      public:

        std::vector< info >  m_Systems;

        template<typename T_SYSTEM>
        void RegisterSystem()
        {
          m_Systems.push_back(
            info{
                std::make_unique< details::CompletedSystem<T_SYSTEM> >(),
                [](SystemBase& system, EntityManager::EntityManager& GM)
                {
                  static_cast<details::CompletedSystem<T_SYSTEM>&>(system).Run(GM);
                }
                });
        }

        void Run(EntityManager::EntityManager& GameMgr)
        {
          for (const auto& S : m_Systems)
            (*S.m_callRun)(*S.m_sys.get(), GameMgr);
        }
      };
    }
	}
}


