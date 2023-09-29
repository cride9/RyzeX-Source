#include "../hooks.h"

void __fastcall h::hkTraceRay( void* ecx, void* edx, const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, Trace_t* pTrace )
{
	static auto original = detour::traceRay.GetOriginal<decltype( &h::hkTraceRay )>( );
	
	// the way we set this variable makes it unnecessary 
	if (!pTrace /*|| !g_Client.m_Autowalling*/)
		return original( ecx, edx, ray, fMask, pTraceFilter, pTrace );

	original( ecx, edx, ray, fMask, pTraceFilter, pTrace );
	pTrace->surface.uFlags |= SURF_SKY;
}