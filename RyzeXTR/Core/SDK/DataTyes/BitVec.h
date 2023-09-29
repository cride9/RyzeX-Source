#pragma once
#include <cstddef>
// @source: master/public/bitvec.h

template <std::size_t N>
class CBitVec
{
	enum : std::uint32_t
	{
		BITS_PER_INT_LOG2 = 5U,
		BITS_PER_INT = 32U
	};

public:

	[[nodiscard]] std::uint32_t operator[](std::size_t nIndex) const
	{
		assert(nIndex < N); // bit index is out of range
		return (arrUInts[nIndex >> BITS_PER_INT_LOG2] & (1U << (nIndex & (BITS_PER_INT - 1U))));
	}

	[[nodiscard]] __forceinline std::uint32_t* Base()
	{
		return arrUInts;
	}

	[[nodiscard]] __forceinline const std::uint32_t* Base() const
	{
		return arrUInts;
	}

	__forceinline void Set(const std::size_t nBit)
	{
		assert(nBit < N); // bit index is out of range
		arrUInts[nBit >> BITS_PER_INT_LOG2] |= (1U << (nBit & (BITS_PER_INT - 1U)));
	}

	__forceinline void Clear(const std::size_t nBit)
	{
		assert(nBit < N); // bit index is out of range
		arrUInts[nBit >> BITS_PER_INT_LOG2] &= ~(1U << (nBit & (BITS_PER_INT - 1U)));
	}

	[[nodiscard]] __forceinline std::uint32_t Get(const std::size_t nBit) const
	{
		assert(nBit < N); // bit index is out of range
		return (arrUInts[nBit >> BITS_PER_INT_LOG2] & (1U << (nBit & (BITS_PER_INT - 1U))));
	}

	[[nodiscard]] __forceinline bool IsBitSet(const std::size_t nBit) const
	{
		assert(nBit < N); // bit index is out of range
		return (arrUInts[nBit >> BITS_PER_INT_LOG2] & (1U << (nBit & (BITS_PER_INT - 1U)))) != 0U;
	}

private:
	std::uint32_t arrUInts[(N + (BITS_PER_INT - 1U)) / BITS_PER_INT] = { };
};
