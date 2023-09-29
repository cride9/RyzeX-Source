#pragma once
#include "../../utilities.h"

using MemAllocFailHandler_t = std::size_t(__cdecl*)(std::size_t);

class IMemAlloc
{
public:
	virtual ~IMemAlloc() { }

	void* Alloc(std::size_t nSize, const char* szFileName, int nLine)
	{
		// @ida: tier0.dll -> "FF 10 85 C0 74"
		return util::CallVFunc<void*>(this, 0U, nSize, szFileName, nLine);
	}

	void* Alloc(std::size_t nSize)
	{
		// @ida: tier0.dll -> U8["50 FF 52 ? 8B D0" + 0x3] / sizeof(std::uintptr_t)
		return util::CallVFunc<void*>(this, 1U, nSize);
	}

	void* Realloc(void* pMemory, std::size_t nSize, const char* szFileName, int nLine)
	{
		return util::CallVFunc<void*>(this, 2U, pMemory, nSize, szFileName, nLine);
	}

	void* Realloc(void* pMemory, std::size_t nSize)
	{
		// @ida: tier0.dll -> U8["FF 52 ? 8B 56" + 0x2]
		return util::CallVFunc<void*>(this, 3U, pMemory, nSize);
	}

	void Free(void* pMemory, const char* szFileName, int nLine)
	{
		util::CallVFunc<void>(this, 4U, pMemory, szFileName, nLine);
	}

	void Free(void* pMemory)
	{
		// @ida: tier0.dll -> U8["FF 52 ? 8B C6 85 F6 75 E7 5E 68" + 0x2] / sizeof(std::uintptr_t)
		util::CallVFunc<void>(this, 5U, pMemory);
	}

	// returns size of a particular allocation
	std::size_t GetSize(void* pMemory)
	{
		// @ida: client.dll -> U8["A1 ? ? ? ? 83 C4 04 8B 08 6A 00 8B 01 FF 50" + 0x10] / sizeof(std::uintptr_t) @xref: "Heap Used:\n"
		return util::CallVFunc<std::size_t>(this, 8U, pMemory);
	}
};
