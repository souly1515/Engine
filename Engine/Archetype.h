#pragma once
#include <memory>
#include <deque>
#include <cassert>
#include <Windows.h>


#include "MetaHelpers.h"
#include "Entity.h"
#include "../dependencies/xcore/src/xcore.h"

namespace Engine
{
  namespace Archetype
  {
    using ChunkIndex = uint32_t;
    constexpr size_t MAX_CHUNK_SIZE = 1ull << 32;
    constexpr size_t SingleCommitSize = 1ull << 12;

    struct Chunk
    {
      char* data;
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
      // dont need this anymore
      //static std::unique_ptr<ChunkComponentInfo[]> m_componentInfo;

      /*
      template<typename C>
      constexpr uint32_t GetOffSet(size_t sizeofChunk)
      {
        // compile time assert
        static_assert(sizeof...(C) > 0);

        static constexpr std::array align{ alignof(C) ... };
        static constexpr std::array size{ sizeof(C) ... };

        static constexpr size_t minSize = Sum<sizeof(C)...>;

        //constexpr size_t maxAlign = std::max(align);

        // what % of the max size would the total size take
        //constexpr float eleMultiplier = (sizeof(C) / alignof(C) + 1) * alignof(C) / maxAlign + ...;

        // start from largest possible and 
        // move down from there
        for (int i = sizeofChunk / minSize; i > 0; --i)
        {
          int nK = 0;
          for (auto k = 0; k < sizeof...(C); ++k)
          {
            // get alignment between blocks
            int blockAlign = 0;

            // if its the last block then there would be no alignment needs
            if ((k + 1) < sizeof...(C))
              // there is an assumption here
              // where there would be no padding if the
              // alignment for the next block is smaller then the current 1
              // which would be fine if all alignment is pow of 2s 1,2,4,8,16 etc
              blockAlign = (align[k + 1] - align[k]) > 0 ? (align[k + 1] - align[k]) : 0;


            nK += ((size[k] / align[k]) + 1) * i + blockAlign;
          }

          if (nK < sizeofChunk)
            return i;
        }
        return -1;
      }
      */

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
        while (endIndex * compSize >= CommitedMemory)
        {
          VirtualAlloc(data + CommitedMemory, SingleCommitSize, MEM_COMMIT, PAGE_READWRITE);
          CommitedMemory += SingleCommitSize;
        }
        return endIndex++;
      }

      void RemoveEntity(ChunkIndex index)
      {
        //swap the components here
        // since they are references they will swap the contents of the pointers
        // as well
        std::swap(GetComponent(index), GetComponent(--endIndex));
      }


      COMPONENT& GetComponent(ChunkIndex entity)
      {
        return *(reinterpret_cast<COMPONENT*>(data + entity * compSize));
      }
    };


    template<typename Component>
    struct Archetype_Intermediate
    {
      Chunk_Impl<Component> chunk;
      ChunkIndex AddEntity_helper()
      {
        return chunk.AddEntity();;
      }

      Component& GetComponent(ChunkIndex index)
      {
        return chunk.GetComponent(index);
      }
    };
    /*
    namespace details
    {
      template<typename Component, typename Archetype_t, typename = std::enable_if_t<std::negation_v<std::is_pointer<Component>::type>>>
      Component& GetComponent(Archetype_t& archetype, ChunkIndex index)
      {
        return archetype.GetComponent(index);
      }

      template<typename Component, typename Archetype_t,
        std::enable_if_t<std::is_base_of_v<Archetype_Intermediate<Component>, Archetype_t>, bool> = true,
        std::enable_if_t<std::is_pointer_v<Component>, bool> = true>
        Component* GetComponent(Archetype_Intermediate<Component>& archetype, ChunkIndex index)
      {
        return &archetype->GetComponent(index);
      }

      template<typename Component, typename Archetype_t,
        typename = std::enable_if_t<std::negation_v<typename std::is_base_of<Archetype_Intermediate<Component>, Archetype_t>::type>>,
        typename = std::enable_if_t<std::is_pointer_v<Component>>>
        Component* GetComponent(Archetype_Intermediate<Component>& archetype, ChunkIndex index)
      {
        return nullptr;
      }

    }
    */
    struct Archetype
    {
      size_t entityNum = 0;
      virtual ~Archetype() = default;
      
      template<typename Component, typename = std::enable_if_t<std::negation_v<std::is_pointer<Component>::type>>>
      Component& GetComponent(ChunkIndex index)
      {
        return dynamic_cast<Archetype_Intermediate<std::decay_t<Component>>*>(this)
          ->GetComponent(index);
      }

      template<typename Component, typename = std::enable_if_t<std::is_pointer_v<Component>>>
      Component* GetComponent(ChunkIndex index)
      {
        auto* temp = dynamic_cast<Archetype_Intermediate<std::decay_t<Component>>*>(this);

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
        using func_traits = xcore::function::traits<Functor>;

        for (int i = 0; i < entityNum; ++i)
        {
          // getcomponent<arg1> ;
          RunWithFunctor(func, i, reinterpret_cast<func_traits::args_tuple*>(nullptr));
        }
      }
    };

    template<typename... COMPONENTS>
    struct Archetype_Impl : public Archetype, public Archetype_Intermediate<COMPONENTS>...
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

      Archetype_Impl()
      {
      }

      size_t firstEmptyChunk = 0;

      ChunkIndex AddEntity()
      {
        if constexpr (sizeof...(COMPONENTS) > 0)
        {
          return AddEntity_helper<0, COMPONENTS...>();
        }
        // no components so no index
        return 0;
      }

      void RemoveEntity(size_t index_to_remove)
      {
        //remove entity from chunk;

        --entityNum;
      }
    };

    template<>
    struct Archetype_Impl <> : public Archetype
    {
      Archetype_Impl()
      {
      }

      size_t firstEmptyChunk = 0;

      ChunkIndex AddEntity()
      {
        // no components so no index
        return 0;
      }

      void RemoveEntity(size_t index_to_remove)
      {
        // nothing to do as there are no components
      }
    };

    template<>
    struct Archetype_Impl <void> : public Archetype_Impl <>
    {
    };
  }
}