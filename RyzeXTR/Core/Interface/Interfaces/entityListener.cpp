#include "entityListener.h"
#include "../../globals.h"
#include "../../Hooks/hooks.h"

void CEntityListener::Setup()
{
	i::EntityList->AddListenerEntity(this);
	util::Print("Entitylistener initialized");
}

void CEntityListener::Destroy()
{
	i::EntityList->RemoveListenerEntity(this);
}

void CEntityListener::OnEntityCreated(CBaseEntity* pEntity)
{
	if (pEntity == nullptr)
		return;

	const int nIndex = pEntity->EntIndex();

	if (nIndex < 0)
		return;

	CBaseClient* pClientClass = pEntity->GetClientClass();

	if (pClientClass == nullptr)
		return;

	// entity class check
	switch (pClientClass->nClassID)
	{
	case EClassIndex::CCSPlayer:
		//vecEntities.emplace_back(pEntity, nIndex);

		if (!detour::estimateAbsVelocity.IsHooked())
			h::HookTable(detour::estimateAbsVelocity, pEntity, 145, &h::hkEstimateAbsVelocity);

		break;
	default:
		break;
	}
}

void CEntityListener::OnEntityDeleted(CBaseEntity* pEntity)
{
	if (pEntity == nullptr)
		return;

	const int nIndex = pEntity->EntIndex();

	if (nIndex < 0)
		return;

	if (const auto it = std::ranges::find(vecEntities, nIndex, &EntityObject_t::nIndex); it != vecEntities.end())
		vecEntities.erase(it);
}