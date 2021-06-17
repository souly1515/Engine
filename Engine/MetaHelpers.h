#pragma once

#include <tuple>
#include <utility>


template <size_t numer1, size_t... numer>
struct MetaNumer
{
	MetaNumer() = default;
	static constexpr size_t GetNumer1()
	{
		return numer1;
	}

	static constexpr std::integer_sequence<size_t, numer...> GetOther()
	{
		return std::make_index_sequence<numer...>();
	}
};

template<size_t... numer>
constexpr MetaNumer<numer...> Make_Numer(std::integer_sequence<size_t, numer...> sequence)
{
	return MetaNumer<numer...>();
}

template <typename numer, size_t denom1,  size_t... denom>
struct MetaFraction
{
	template<size_t Index>
	static constexpr std::pair<size_t, size_t> GetFraction()
	{
		static_assert ((sizeof...(denom) >= Index), "Index is larger then number of parameters");

		if constexpr (Index)
		{
			//return MetaFraction<decltype (Make_Numer(numer::GetOther())), denom...>::GetFraction<Index - 1> (1);
			return { 0,0 };
		}
		else
		{
			return { numer::GetNumer1(), denom1 };
		}
	}
};

template<size_t... Vals>
constexpr size_t Sum()
{
	return (Vals + ...);
}

