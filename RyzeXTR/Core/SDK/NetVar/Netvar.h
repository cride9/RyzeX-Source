#pragma once
#include <unordered_map>
#include "fnv.h"
#include "datatable.h"
#include "../../Interface/interfaces.h"
#include "../datamap.h"

namespace n {

	// yoinked from qo0, cry about it
	struct NetvarObject_t
	{
		RecvProp_t* pRecvProp = nullptr;
		std::uintptr_t uOffset = 0U;
	};

	void SetupNetvars();
	void DumpNetvars(const char* szBase, RecvTable_t* cTable, uint32_t offset = 0);
	std::uintptr_t FindInDataMap(DataMap_t* pMap, const uint32_t uFieldHash);

	inline std::unordered_map<uint32_t, NetvarObject_t> netvars = {};
}