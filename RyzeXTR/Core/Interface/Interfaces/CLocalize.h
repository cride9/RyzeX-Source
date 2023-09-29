#pragma once
class ILocalizationChangeCallback
{
public:
	virtual void OnLocalizationChanged() = 0;
};

class ILocalizeTextQuery
{
public:
	virtual int ComputeTextWidth(const wchar_t* pString) = 0;
};
const unsigned short INVALID_STRING_INDEX = (unsigned short)-1;
typedef unsigned int LocalizeStringIndex_t;

class KeyValues;

class ILocalize
{
public:

	wchar_t* Find(const char* tokenName) {
		using original_fn = wchar_t* (__thiscall*)(ILocalize*, const char*);
		return (*(original_fn**)this)[12](this, tokenName);
	}
};