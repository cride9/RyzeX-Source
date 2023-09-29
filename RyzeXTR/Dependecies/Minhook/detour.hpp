#pragma once
#include "minhook.h"
#include <list>
#include "../../Core/SDK/X86RetSpoof.h"

/*
 * detour hooking implementation using minhook
 * @credits: https://github.com/TsudaKageyu/minhook
 */

class CDetourHook
{
public:
	CDetourHook() = default;

	explicit CDetourHook(void* pFunction, void* pDetour)
		: pBaseFn(pFunction), pReplaceFn(pDetour) { }

	~CDetourHook()
	{
		this->Remove();
	}

	/* first hook setup */
	bool Create(void* pFunction, void* pDetour)
	{
		pBaseFn = pFunction;

		if (pBaseFn == nullptr)
			return false;

		pReplaceFn = pDetour;

		if (pReplaceFn == nullptr)
			return false;

		MH_STATUS status = MH_CreateHook(pBaseFn, pReplaceFn, &pOriginalFn);

		if (status == MH_OK)
		{
			if (!this->Replace())
				return false;
		}
		else
		{
			return false;
		}

		allHooks.push_back(this);
		return true;
	}

	/* replace function to our modified */
	bool Replace()
	{
		// check is hook be created
		if (pBaseFn == nullptr)
			return false;

		// check is function not already hooked
		if (bIsHooked)
			return false;

		MH_STATUS status = MH_EnableHook(pBaseFn);

		if (status == MH_OK)
			// switch hook state
			bIsHooked = true;
		else
		{
			return false;
		}

		return true;
	}

	/* remove hook fully */
	bool Remove()
	{
		// restore it at first
		if (!this->Restore())
			return false;

		MH_STATUS status = MH_RemoveHook(pBaseFn);

		if (status == MH_OK)
			return true;
		else
		{
			return false;
		}

		return false;
	}

	static bool RemoveAll() {
		for (CDetourHook* hook : allHooks)
		{
			if (!hook->Remove())
				return false;
		}

		return true;
	}

	/* replace swaped modified function back to original */
	bool Restore()
	{
		// check is function hooked
		if (!bIsHooked)
			return false;

		MH_STATUS status = MH_DisableHook(pBaseFn);

		if (status == MH_OK)
			// switch hook state
			bIsHooked = false;
		else
		{
			return false;
		}

		return true;
	}

	/* get original function pointer (not a call!) */
	template <typename Fn>
	Fn GetOriginal()
	{
		return (Fn)pOriginalFn;
	}

	template <typename Fn, typename... StackArgs>
	Fn CallOriginal(uintptr_t iGadget, void* ecx, void* edx, StackArgs... args) {

		return x86RetSpoof::invokeFastcall<Fn>(reinterpret_cast<std::uintptr_t>(ecx), reinterpret_cast<std::uintptr_t>(edx), reinterpret_cast<std::uintptr_t>(pOriginalFn), iGadget, args...);
	}

	template <typename Fn, typename... StackArgs>
	Fn CallOriginal(uintptr_t iGadget, void* ecx, int edx, StackArgs... args) {

		return x86RetSpoof::invokeFastcall<Fn>(reinterpret_cast<std::uintptr_t>(ecx), edx, reinterpret_cast<std::uintptr_t>(pOriginalFn), iGadget, args...);
	}

	template <typename Fn, typename... StackArgs>
	Fn CallOriginalCdecl(uintptr_t iGadget, StackArgs... args) {

		return x86RetSpoof::invokeCdecl<Fn>(reinterpret_cast<std::uintptr_t>(pOriginalFn), iGadget, args...);
	}

	/* get hook state */
	inline bool IsHooked()
	{
		return bIsHooked;
	}

private:
	inline static std::list<CDetourHook*> allHooks;
	// Values
	/* hook state */
	bool bIsHooked = false;
	/* function base address */
	void* pBaseFn = nullptr;
	/* in future that is being modified and replace original */
	void* pReplaceFn = nullptr;
	/* saved function to get available restore hook later */
	void* pOriginalFn = nullptr;
};
