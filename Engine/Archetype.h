#pragma once
#include <memory>
#include <deque>
#include <cassert>
#include <Windows.h>


#include "Entity.h"
//#include "../dependencies/xcore/src/xcore.h"
#include "func_traits.h"

namespace Engine
{
  namespace Archetype
  {
    using ChunkIndex = uint32_t;
    constexpr size_t MAX_CHUNK_SIZE = 1ull << 32;
    constexpr size_t SingleCommitSize = 1ull << 12;

    struct Chunk
    {
      char* data = nullptr;
      Chunk();
      virtual ~Chunk();
    };

    struct ChunkComponentInfo
    {
      size_t size_of_struct;
      size_t offset_from_base;
    };

    template<typename  COMPONENT>
    struct Chunk_Impl : public Chunk
    {
      // pointing to the 1 past end index
      ChunkIndex endIndex = 0;

      uint64_t CommitedMemory = 0;
      /*
      this is explicitly for structs that may end on a different
      alignment that it starts with
      like 
      struct 
      {
        double a;
        int b;
      };
      the above depending on the compiler may be 12 bytes or 16
      so it may end on an alignment different from the expected
      */
      static constexpr size_t compSize =
        sizeof(COMPONENT) % alignof(COMPONENT) ?
        (sizeof(COMPONENT) / alignof(COMPONENT) + 1) * alignof(COMPONENT) :
        sizeof(COMPONENT);

      Chunk_Impl() = default;

      // gets number of compnents avaiable before the next allocation
      size_t GetNumAvail()
      {
        return (CommitedMemory / sizeof(COMPONENT)) - endIndex;
      }

      // returns index id
      ChunkIndex AddEntity()
      {
        while ((endIndex + 1) * compSize >= CommitedMemory)
        {
          VirtualAlloc(data + CommitedMemory, SingleCommitSize, MEM_COMMIT, PAGE_READWRITE);
          CommitedMemory += SingleCommitSize;
        }
        return endIndex++;
      }

      ChunkIndex DeleteEntity(ChunkIndex index)
      {
        //swap the components here
        // since they are references they will swap the contents of the pointers
        // as well
        auto& comp = GetComponent(index);
        if (index == --endIndex)
        {
          comp.~COMPONENT();
          return index;
        }
        auto& end = GetComponent(endIndex);
        std::swap(comp, end);
        end.~COMPONENT();
        return endIndex;
      }


      COMPONENT& GetComponent(ChunkIndex entity)
      {
        return *(reinterpret_cast<COMPONENT*>(data + entity * compSize));
      }
    };

    template<typename Component>
    struct Archetype_Intermediate
    {
      ~Archetype_Intermediate()
      {
        for (unsigned i = 0; i < chunk.endIndex; ++i)
        {
          GetComponent(i).~Component();
        }
      }

      Chunk_Impl<Component> chunk;
      ChunkIndex AddEntity_helper()
      {
        return chunk.AddEntity();;
      }

      Component& GetComponent(ChunkIndex index)
      {
        return chunk.GetComponent(index);
      }

      ChunkIndex DeleteEntity(ChunkIndex index)
      {
        return chunk.DeleteEntity(index);
      }
    };

    struct Archetype
    {
      size_t entityNum = 0;
      virtual ~Archetype() = default;
      
      template<typename Component, typename = std::enable_if_t<std::negation_v<std::is_pointer<Component>::type>>>
      std::decay_t<Component>& GetComponent(ChunkIndex index)
      {
        return dynamic_cast<Archetype_Intermediate<std::decay_t<Component>>*>(this)
          ->GetComponent(index);
      }

      template<typename Component, typename = std::enable_if_t<std::is_pointer_v<Component>>>
      std::remove_pointer_t<std::decay_t<Component>>* GetComponent(ChunkIndex index)
      {
        auto* temp = dynamic_cast<Archetype_Intermediate<std::remove_pointer_t<std::decay_t<Component>>>*>(this);

        if(temp)
          return &temp->GetComponent(index);

        return nullptr;
      }

      template <typename Functor, typename... ArgType>
      void RunWithFunctor(Functor& func, ChunkIndex index, std::tuple<ArgType...>*)
      {
        func(GetComponent<ArgType>(index)...);
      }

      template<typename Functor>
      void RunWithFunctor(Functor& func)
      {
        // get the type of arguments
        using func_traits = Engine::traits<Functor>;

        for (int i = 0; i < entityNum; ++i)
        {
          // getcomponent<arg1> ;
          RunWithFunctor(func, i, reinterpret_cast<func_traits::args_tuple*>(nullptr));
        }
      }
      virtual ChunkIndex DeleteEntity(ChunkIndex index) = 0;
    };

    template<typename... COMPONENTS>
    struct Archetype_Impl : public Archetype, public Archetype_Intermediate<EntityComponent>, public Archetype_Intermediate<COMPONENTS>...
    {
    private:

      template<size_t I, typename Component, typename... Component_other>
      ChunkIndex AddEntity_helper(ChunkIndex expectedIndex)
      {
        auto temp = dynamic_cast<Archetype_Intermediate<Component>*>(this)->AddEntity_helper();
        assert(temp == expectedIndex);

        if constexpr(sizeof...(Component_other) > 0)
          AddEntity_helper<I + 1, Component_other...>(expectedIndex);

        return expectedIndex;
      }

      // start no component specialisation
      template<size_t I>
      ChunkIndex AddEntity_helper()
      {
        return 0;
      }

      // start
      template<size_t I, typename Component, typename... Component_other>
      ChunkIndex AddEntity_helper()
      {
        auto temp = dynamic_cast<Archetype_Intermediate<Component>*>(this)->AddEntity_helper();
        if constexpr (sizeof...(Component_other) != 0)
        {
          AddEntity_helper<I + 1, Component_other...>(temp);
        }
        ++entityNum;
        return temp;
      }

    public:
      //std::tuple<Chunk_Impl<COMPONENTS>...> chunkList;

      virtual ChunkIndex DeleteEntity(ChunkIndex index) override
      {
        ChunkIndex idx = index;
        if constexpr (sizeof...(COMPONENTS) > 0)
        {
          idx = (dynamic_cast<Archetype_Intermediate<EntityComponent>*>(this)->DeleteEntity(index));
          (dynamic_cast<Archetype_Intermediate<COMPONENTS>*>(this)->DeleteEntity(index),...);
        }
        --entityNum;
        return idx;
      }

      Archetype_Impl()
      {
      }

      size_t firstEmptyChunk = 0;

      ChunkIndex AddEntity()
      {
        if constexpr (sizeof...(COMPONENTS) > 0)
        {
          return AddEntity_helper<0, EntityComponent, COMPONENTS...>();
        }
        // no components so no index
        return 0;
      }
    };

  }
}