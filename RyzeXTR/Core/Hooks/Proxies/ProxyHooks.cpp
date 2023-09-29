#include "ProxyHooks.h"
#include "../../Features/Changers/SkinChanger.h"
#include "../../SDK/Entity.h"
#include "../../globals.h"
#include "../../Features/Rage/exploits.h"

bool p::Setup()
{
	// get viewmodel m_nSequence Netvar prop
	RecvProp_t* pViewModelSequence = n::netvars[ fnv::Hash( "CBaseViewModel->m_nSequence" ) ].pRecvProp;
	if ( pViewModelSequence == nullptr )
		return false;

	hkBaseViewModelSequence = std::make_shared<CRecvPropHook>( pViewModelSequence, BaseViewModelSequence );

	RecvProp_t* pTickbaseProp = n::netvars[fnv::Hash("CBasePlayer->m_nTickBase")].pRecvProp;
	if (pTickbaseProp == nullptr)
		return false;

	hkTickbaseProxy = std::make_shared<CRecvPropHook>(pTickbaseProp, TickbaseProxy);

	return true;
}

void p::Destroy()
{
	// @note: as example
	hkBaseViewModelSequence->Restore();
	hkTickbaseProxy->Restore();
}

void p::BaseViewModelSequence(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	CBaseEntity* pLocal = CBaseEntity::GetLocalPlayer();

	const auto oSequence = hkBaseViewModelSequence->GetOriginal();

	// sanity checks
	if (pLocal == nullptr || !pLocal->IsAlive())
		return oSequence(pData, pStruct, pOut);

	// get modifiable proxy data
	auto ProxyData = const_cast<CRecvProxyData*>(pData);
	// get viewmodel pointer
	auto ViewModel = static_cast<CBaseViewModel*>(pStruct);
	// get owner from viewmodel pointer
	unsigned int OwnerHandle = ViewModel->GetOwnerHandle();

	if (ViewModel && OwnerHandle)
	{
		// get owner of current viewmodel
		CBaseEntity* Owner = static_cast<CBaseEntity*>(i::EntityList->GetClientEntityFromHandle(OwnerHandle));

		if (Owner == g::pLocal)
		{
			// Get the filename of the current view model.
			const Model_t* KnifeModel = i::ModelInfo->GetModel(ViewModel->GetModelIndex());
			const char* ModelName = i::ModelInfo->GetModelName(KnifeModel);
			// set proxy data
			ProxyData->Value.Int = skinChanger.GetNewAnimation( fnv::Hash( ModelName ), ProxyData->Value.Int);
		}
	}

	oSequence( ProxyData, pStruct, pOut );
}

void p::TickbaseProxy(const CRecvProxyData* pData, void* pStruct, void* pOut) {

	const auto oTickbaseProxy = hkTickbaseProxy->GetOriginal();
	oTickbaseProxy(pData, pStruct, pOut);
}