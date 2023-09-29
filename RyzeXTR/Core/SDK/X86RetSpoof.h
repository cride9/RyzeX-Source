#pragma once

#include <cstdint>
#include <utility>

namespace x86RetSpoof
{
	namespace detail
	{
		struct Context;
	}

	template <typename ReturnType, typename... StackArgs>
	ReturnType invokeFastcall(std::uintptr_t ecx, std::uintptr_t edx, std::uintptr_t functionAddress, std::uintptr_t gadgetAddress, StackArgs... stackArgs) noexcept
	{
		detail::Context context;
		return invokeFastcall<ReturnType, StackArgs...>(ecx, edx, functionAddress, &context, gadgetAddress, stackArgs...);
	}

	template <typename ReturnType, typename... StackArgs>
	ReturnType invokeThiscall(std::uintptr_t ecx, std::uintptr_t functionAddress, std::uintptr_t gadgetAddress, StackArgs... stackArgs) noexcept
	{
		return invokeFastcall<ReturnType, StackArgs...>(ecx, 0, functionAddress, gadgetAddress, stackArgs...);
	}

	template <typename ReturnType, typename... StackArgs>
	ReturnType invokeStdcall(std::uintptr_t functionAddress, std::uintptr_t gadgetAddress, StackArgs... stackArgs) noexcept
	{
		return invokeThiscall<ReturnType, StackArgs...>(0, functionAddress, gadgetAddress, stackArgs...);
	}

	template <typename ReturnType, typename... StackArgs>
	ReturnType invokeCdecl(std::uintptr_t functionAddress, std::uintptr_t gadgetAddress, StackArgs... stackArgs) noexcept
	{
		detail::Context context;
		return invokeCdecl<ReturnType, StackArgs...>(functionAddress, &context, gadgetAddress, stackArgs...);
	}

	namespace detail
	{
		struct Context {
			std::uintptr_t ebxBackup;
			std::uintptr_t addressToJumpToInGadget;
			std::uintptr_t invokerReturnAddress;
		};

		template <typename ReturnType, typename... StackArgs>
		__declspec(naked) ReturnType __fastcall invokeFastcall(std::uintptr_t /*ecx*/, std::uintptr_t /*edx*/, std::uintptr_t /*functionAddress*/, Context* /*context*/, std::uintptr_t /*gadgetAddress*/, StackArgs... /*stackArgs*/) noexcept
		{
			__asm {
				mov eax, [esp + 8] // load a pointer to context into eax
				mov[eax], ebx // save ebx in context.ebxBackup
				lea ebx, returnHereFromGadget // load the address of the label we want the gadget to jump to
				mov[eax + 4], ebx // save the address of 'returnHereFromGadget' in context.addressToJumpToInGadget
				pop dword ptr[eax + 8] // pop return address from stack into context.invokerReturnAddress

				lea ebx, [eax + 4] // load the address of context.addressToJumpToInGadget to ebx
				ret 4 // pop 'functionAddress' from stack and jump to it, skip 'context' on stack; esp will point to the spoofed return address (gadgetAddress)

				returnHereFromGadget:
				push[ebx + 4] // restore context.invokerReturnAddress as a return address
					mov ebx, [ebx - 4] // restore ebx from context.ebxBackup 
					ret
			}
		}

		template <typename ReturnType, typename... StackArgs>
		__declspec(naked) ReturnType __cdecl invokeCdecl(std::uintptr_t /*functionAddress*/, Context* /*context*/, std::uintptr_t /*gadgetAddress*/, StackArgs... /*stackArgs*/) noexcept
		{
			__asm {
				mov eax, [esp + 8] // load a pointer to context into eax
				mov[eax], ebx // save ebx in context.ebxBackup
				lea ebx, returnHereFromGadget // load the address of the label we want the gadget to jump to
				mov[eax + 4], ebx // save the address of 'returnHereFromGadget' in context.addressToJumpToInGadget
				pop dword ptr[eax + 8] // pop return address from stack into context.invokerReturnAddress

				lea ebx, [eax + 4] // load the address of context.addressToJumpToInGadget to ebx
				ret 4 // pop 'functionAddress' from stack and jump to it, skip 'context' on stack; esp will point to the spoofed return address (gadgetAddress)

				returnHereFromGadget:
				sub esp, 12
					push[ebx + 4] // restore context.invokerReturnAddress as a return address
					mov ebx, [ebx - 4] // restore ebx from context.ebxBackup 
					ret
			}
		}
	}
}