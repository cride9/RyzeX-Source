#pragma once
#include "../../SDK/DataTyes/UtlVector.h"

class IClientNetworkable;
class IClientUnknown;
class IClientEntity;
class CBaseEntity;

// Cached info for networked entities.
// NOTE: Changing this changes the interface between engine & client
struct EntityCacheInfo_t
{
	// Cached off because GetClientNetworkable is called a *lot*
	IClientNetworkable* m_pNetworkable;
	unsigned short m_BaseEntitiesIndex;	// Index into m_BaseEntities (or m_BaseEntities.InvalidIndex() if none).
	unsigned short m_bDormant;	// cached dormant state - this is only a bit
};

class IClientEntityListener
{
public:
	virtual void OnEntityCreated(CBaseEntity* pEntity) { }
	virtual void OnEntityDeleted(CBaseEntity* pEntity) { }
};

// https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/public/icliententitylist.h#L37
class IClientEntityList {

public:

	// Get IClientNetworkable interface for specified entity
	virtual IClientNetworkable* GetClientNetworkable(int entnum) = 0;
	virtual IClientNetworkable* GetClientNetworkableFromHandle(std::uintptr_t hEnt) = 0;
	virtual IClientUnknown* GetClientUnknownFromHandle(std::uintptr_t hEnt) = 0;

	// NOTE: This function is only a convenience wrapper.
	// It returns GetClientNetworkable( entnum )->GetIClientEntity().
	virtual IClientEntity* GetClientEntity(int entnum) = 0;
	virtual IClientEntity* GetClientEntityFromHandle(std::uintptr_t hEnt) = 0;

	// Returns number of entities currently in use
	virtual int					NumberOfEntities(bool bIncludeNonNetworkable) = 0;

	// Returns highest index actually used
	virtual int					GetHighestEntityIndex(void) = 0;

	// Sizes entity list to specified size
	virtual void				SetMaxEntities(int maxents) = 0;
	virtual int					GetMaxEntities() = 0;
	virtual EntityCacheInfo_t* GetClientNetworkableArray() = 0;

	void AddListenerEntity(IClientEntityListener* pListener)
	{
		vecEntityListeners.AddToTail(pListener);
	}

	void RemoveListenerEntity(IClientEntityListener* pListener)
	{
		vecEntityListeners.FindAndRemove(pListener);
	}

	CUtlVector<IClientEntityListener*> vecEntityListeners;

private:

};