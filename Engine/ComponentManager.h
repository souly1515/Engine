#pragma once
#include <type_traits>
#include "Bitset.h"

namespace Engine
{
	namespace Component
	{

		using ComponentBitset = Tools::Bitset<2>;
		namespace Details
		{
			struct info
			{
				mutable int m_UID;
				size_t m_size;
			};

			template <typename T>
			constexpr info CreateInfo()
			{
				return info
				{
					-1,
					sizeof(T)
				};
			}
			template <typename T>
			static constexpr auto info_v = Details::CreateInfo<T>();
		} // details

		// std::decay -> remove keyword const if needed
		// this will create multiple references to the same structure
		template <typename T>
		constexpr auto& component_info_v = Component::Details::template info_v<std::decay<T>>;

		template <typename T>
		static constexpr int& bitOffset_v = component_info_v<T>.m_UID;

		class ComponentManager
		{
		public:

			template<typename T_COMPONENT>
			void RegisterComponent(void) noexcept
			{
				if (component_info_v<T_COMPONENT>.m_UID == -1)
					component_info_v<T_COMPONENT>.m_UID = m_uniqueID++;
			}
			int m_uniqueID = 0;

			
		};


	}
}

