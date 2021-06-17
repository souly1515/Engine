#pragma once
#include <type_traits>

namespace Engine
{
	namespace Tools
	{
		template <unsigned multiplier = 2, typename Underlying = unsigned>
		class Bitset
		{

		public:
			const Underlying size = multiplier;
			Underlying data[multiplier];

			Bitset()
			{

			}

			Bitset(uint32_t offset)
			{
				Set(offset);
			}

			Bitset(const Bitset& rhs)
			{
				int i = 0;
				for (auto& d : data)
				{
					d = rhs.data[i++];
				}
			}

			Underlying* GetUnderlying()
			{
				return data;
			}

			constexpr unsigned GetLength() const
			{
				return multiplier * Underlying;
			}

			void Set(unsigned index)
			{
				data[index / sizeof(Underlying)] |= 1ull << (index % sizeof(Underlying));
			}

			void Reset(unsigned index)
			{
				data[index / sizeof(Underlying)] ^= 1ull << (index % sizeof(Underlying));
			}

			void Invert(unsigned index)
			{
				//... will think about this later
			}

			// exact copy
			bool operator==(const Bitset<multiplier, Underlying>& rhs) const
			{
				int i = 0;
				for (auto& d : data)
				{
					if ((rhs.data[i++] ^ d))
						return false;
				}
				return true;
			}

			// not exact copy
			bool operator!=(const Bitset<multiplier, Underlying>& rhs) const
			{
				return !(*this == rhs);
			}

			Bitset<multiplier, Underlying>& operator+=(const Bitset<multiplier, Underlying>& rhs)
			{
				int i = 0;
				for (auto& d : data)
				{
					d |= rhs.data[i++];
				}
				return *this;
			}

			Bitset<multiplier, Underlying>& operator&(const Bitset<multiplier, Underlying>& rhs) const
			{
				int i = 0;
				for (auto& d : data)
				{
					if ((rhs.data[i++] & d))
						return true;
				}
				return false;
			}


			Bitset<multiplier, Underlying>& operator=(const Bitset<multiplier, Underlying>& rhs)
			{
				int i = 0;
				for (auto& d : data)
				{
					d = rhs.data[i++];
				}
				return this;
			}

			operator bool () const
			{
				for (auto& d : data)
				{
					if (d)
						return true;
				}
				return false;
			}
		};

		template <unsigned multiplier, typename Underlying>
		Bitset<multiplier, Underlying> operator+(const Bitset<multiplier, Underlying>& lhs, const Bitset<multiplier, Underlying>& rhs)
		{
			Bitset other{ lhs };
			other += rhs;
			return std::move(other);
		}
	}
}