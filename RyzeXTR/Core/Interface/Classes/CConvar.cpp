#include "CConvar.h"
#include "../interfaces.h"
#include <memoryapi.h>
#include <system_error>
#include <errhandlingapi.h>

class CWrappedProtect
{
public:
	CWrappedProtect(void* pBaseAddress, const std::size_t uLength, const DWORD dwFlags) :
		pBaseAddress(pBaseAddress), uLength(uLength)
	{
		if (!VirtualProtect(pBaseAddress, uLength, dwFlags, &dwOldFlags))
			throw std::system_error(GetLastError(), std::system_category(), "failed to protect region");
	}

	~CWrappedProtect()
	{
		VirtualProtect(pBaseAddress, uLength, dwOldFlags, &dwOldFlags);
	}
private:
	void* pBaseAddress;
	std::size_t		uLength;
	DWORD			dwOldFlags;
};

CSpoofedConVar::CSpoofedConVar(const char* szCVar)
{
	pOriginalCVar = i::ConVar->FindVar(szCVar);
	Spoof();
}

CSpoofedConVar::CSpoofedConVar(CConVar* pCVar)
{
	pOriginalCVar = pCVar;
	Spoof();
}

CSpoofedConVar::~CSpoofedConVar()
{
	if (IsSpoofed())
	{
		// restore values
		SetFlags(iOriginalFlags);
		SetString(szOriginalValue);

		// restore original name
		CWrappedProtect protection = CWrappedProtect{ (LPVOID)pOriginalCVar->szName, 128UL, PAGE_READWRITE };
		strcpy_s(const_cast<char*>(pOriginalCVar->szName), sizeof(szOriginalName), szOriginalName);

		// unregister temporary convar
		i::ConVar->UnregisterConCommand(pDummyCVar);
		free(pDummyCVar);
		pDummyCVar = nullptr;
	}
}

bool CSpoofedConVar::IsSpoofed() const
{
	return pDummyCVar != nullptr;
}

void CSpoofedConVar::Spoof()
{
	if (!IsSpoofed() && pOriginalCVar != nullptr)
	{
		// backup old values
		iOriginalFlags = pOriginalCVar->nFlags;
		strcpy_s(szOriginalName, pOriginalCVar->szName);
		strcpy_s(szOriginalValue, pOriginalCVar->szDefaultValue);
		sprintf_s(szDummyName, 128U, "ryzextr_%s", szOriginalName);

		// create temporary convar
		pDummyCVar = static_cast<CConVar*>(malloc(sizeof(CConVar)));

		if (pDummyCVar == nullptr)
			return;

		memcpy(pDummyCVar, pOriginalCVar, sizeof(CConVar));
		pDummyCVar->pNext = nullptr;

		// register temporary convar
		i::ConVar->RegisterConCommand(pDummyCVar);

		CWrappedProtect protection = CWrappedProtect{ (LPVOID)pOriginalCVar->szName, 128UL, PAGE_READWRITE };
		// rename the convar
		strcpy_s(const_cast<char*>(pOriginalCVar->szName), sizeof(szDummyName), szDummyName);

		SetFlags(0);
	}
}

void CSpoofedConVar::SetFlags(int nFlags) const
{
	if (IsSpoofed())
		pOriginalCVar->nFlags = nFlags;
}

int CSpoofedConVar::GetFlags() const
{
	return pOriginalCVar->nFlags;
}

void CSpoofedConVar::SetInt(int iValue) const
{
	if (IsSpoofed())
		pOriginalCVar->SetValue(iValue);
}

void CSpoofedConVar::SetBool(bool bValue) const
{
	if (IsSpoofed())
		pOriginalCVar->SetValue(bValue);
}

void CSpoofedConVar::SetFloat(float flValue) const
{
	if (IsSpoofed())
		pOriginalCVar->SetValue(flValue);
}

void CSpoofedConVar::SetString(const char* szValue) const
{
	if (IsSpoofed())
		pOriginalCVar->SetValue(szValue);
}
