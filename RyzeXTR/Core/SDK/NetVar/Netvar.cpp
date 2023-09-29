#include "netvar.h"
#include <ctype.h>
#include <format>

void n::SetupNetvars() {

	for (auto BaseClient = i::ClientDll->GetAllClasses(); BaseClient != nullptr; BaseClient = BaseClient->pNext) {

		if (BaseClient->pRecvTable != nullptr) {

			DumpNetvars(BaseClient->szNetworkName, BaseClient->pRecvTable);
		}
	}
	util::Print(XorStr("Netvars initialized"));
}

void n::DumpNetvars(const char* szBase, RecvTable_t* cTable, uint32_t offset) {

	for (int i = 0; i < cTable->nProps; ++i) {

		const auto prop = &cTable->pProps[i];

		if (!prop || isdigit(prop->szVarName[0]))
			continue;

		if (fnv::Hash(prop->szVarName) == fnv::HashConst("baseclass"))
			continue;

		if (prop->iRecvType == ESendPropType::DPT_DATATABLE && prop->pDataTable && prop->pDataTable->szNetTableName[0] == 'D')
			DumpNetvars(szBase, prop->pDataTable, offset + prop->iOffset);

		// concat to our netvar format just by hash
		const uint32_t uTotalHash = fnv::Hash( prop->szVarName, fnv::Hash( "->", fnv::Hash( szBase ) ) );

		netvars[ uTotalHash ] = { prop, offset + prop->iOffset };
	}
}

std::uintptr_t n::FindInDataMap(DataMap_t* pMap, const uint32_t uFieldHash)
{
	while (pMap != nullptr)
	{
		for (int i = 0; i < pMap->nDataFields; i++)
		{
			if (pMap->pDataDesc[i].szFieldName == nullptr)
				continue;

			if (fnv::Hash(pMap->pDataDesc[i].szFieldName) == uFieldHash)
				return pMap->pDataDesc[i].iFieldOffset[TD_OFFSET_NORMAL];

			if (pMap->pDataDesc[i].iFieldType == FIELD_EMBEDDED)
			{
				if (pMap->pDataDesc[i].pTypeDescription != nullptr)
				{
					if (const auto uOffset = FindInDataMap(pMap->pDataDesc[i].pTypeDescription, uFieldHash); uOffset != 0U)
						return uOffset;
				}
			}
		}

		pMap = pMap->pBaseMap;
	}

	return 0U;
}