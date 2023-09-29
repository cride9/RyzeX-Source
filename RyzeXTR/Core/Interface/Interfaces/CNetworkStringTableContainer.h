#pragma once
#include "../../utilities.h"
class INetworkStringTable
{
public:

	virtual					~INetworkStringTable(void) {};

	// Table Info
	virtual const char*		GetTableName(void) const = 0;
	virtual int				GetTableId(void) const = 0;
	virtual int				GetNumStrings(void) const = 0;
	virtual int				GetMaxStrings(void) const = 0;
	virtual int				GetEntryBits(void) const = 0;

	// Networking
	virtual void			SetTick(int tick) = 0;
	virtual bool			ChangedSinceTick(int tick) const = 0;

	// Accessors (length -1 means don't change user data if string already exits)
	virtual int				AddString(bool bIsServer, const char* value, int length = -1, const void* userdata = 0) = 0;

	virtual const char* GetString(int stringNumber) const = 0;
	virtual void			SetStringUserData(int stringNumber, int length, const void* userdata) = 0;
	virtual const void* GetStringUserData(int stringNumber, int* length) const = 0;
	virtual int				FindStringIndex(char const* string) = 0; // returns INVALID_STRING_INDEX if not found

	// Callbacks
	virtual void			SetStringChangedCallback(void* object, void* changeFunc) = 0;
};

class CNetworkStringTableContainer
{
public:
	INetworkStringTable* FindTable(const char* tableName)
	{
		return util::CallVFunc<INetworkStringTable*>(this, 3, tableName);
		//typedef INetworkStringTable* (__thiscall* oFindTable)(void*, const char*);
		//return util::CallVFunc< oFindTable >(this, 3)(this, tableName);
	}
	//virtual void unused0();
	//virtual void unused1();
	//virtual void unused2();
	//virtual INetworkStringTable* FindTable(const char* tableName);
};