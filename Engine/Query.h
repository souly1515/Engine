#pragma once
#include "Bitset.h"
#include "ComponentManager.h"
#include "func_traits.h"

namespace Engine
{
  namespace EntityManager
  {
    class EntityManager;
  }
  namespace Tools
  {
    namespace details
    {
      template <typename T>
      concept has_Execute = requires(T & t, Engine::EntityManager::EntityManager & GM)
      {
        t.Execute(GM);
      };

    }

    struct Query
    {
      template< typename... T_COMPONENTS >
      struct must
      {
        using type = std::tuple<T_COMPONENTS...>;
      };

      template< typename... T_COMPONENTS >
      struct one_of
      {
        using type = std::tuple<T_COMPONENTS...>;
      };

      template< typename... T_COMPONENTS >
      struct none_of
      {
        using type = std::tuple<T_COMPONENTS...>;
      };

      Bitset<2>    m_Must;
      Bitset<2>    m_OneOf;
      Bitset<2>    m_NoneOf;

      template<typename T>
      void SetQueryType()
      {
        auto test = Engine::Component::component_info_v<T>;
        if constexpr (std::is_pointer_v<T>)
        {
          m_OneOf.Set(Engine::Component::component_info_v<std::remove_pointer_t<T>>.m_UID);
        }
        else if constexpr (std::is_reference_v<T>)
        {
          m_Must.Set(Engine::Component::component_info_v<std::remove_reference_t<T>>.m_UID);
        }
        else
        {
          static_assert(false);
        }
      }

      template<typename T_Function>
      void GenerateQueryFromFunction(T_Function&& func)
      {
        // concept testing that it has operator()
        constexpr bool fn = details::has_Execute<T_Function>;
        // if it does not have an execute then do this
        // cause i would need a query for operator()
        if constexpr (!fn)
        {
          using func_traits = Engine::traits<T_Function>;
          [&] <typename... T_Components>(std::tuple<T_Components...>*)
          {
            (SetQueryType<T_Components>(), ...);
          }
          (reinterpret_cast<func_traits::args_tuple*>(nullptr));
        }
      }
      
      
      template<typename... T_Queries>
      void SetFromTuple(std::tuple<T_Queries...>*)
      {
        // T is a templateised type that has a variadic template pack
        auto func = [&]<template<typename ...>class T, typename ... T_Component>(T<T_Component...>*)
        {
          if constexpr (std::is_same_v<T<T_Component...>, Query::one_of<T_Component...>>)
          {
            (m_OneOf.Set(Engine::Component::component_info_v<T_Component>.m_UID), ...);
          }
          else if constexpr (std::is_same_v<T<T_Component...>, Query::must<T_Component...>>)
          {
            (m_Must.Set(Engine::Component::component_info_v<T_Component>.m_UID), ...);
          }
          else if constexpr (std::is_same_v<T<T_Component...>, Query::none_of<T_Component...>>)
          {
            (m_NoneOf.Set(Engine::Component::component_info_v<T_Component>.m_UID), ...);
          }
          else // fail in compilation
            static_assert(false);
        };
        (func(reinterpret_cast<T_Queries*>(nullptr)), ...);
      }

      bool Compare(const Bitset<2>& ArchetypeBits) const noexcept
      {
        bool oneof = !(static_cast<bool>(m_OneOf)); // need to be able to convert bits to bool 

        for (int i = 0; i != ArchetypeBits.size; ++i)
        {
          if (m_NoneOf.data[i] & ArchetypeBits.data[i])
            return false;

          if ((m_Must.data[i] & ArchetypeBits.data[i]) != m_Must.data[i])
            return false;

          oneof |= (bool)(ArchetypeBits.data[i] & m_OneOf.data[i]);

        }
        return oneof;
      }
    };


  }
}