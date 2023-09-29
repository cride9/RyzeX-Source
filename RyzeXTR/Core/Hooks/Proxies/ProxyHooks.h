#pragma once
#include "../../SDK/NetVar/datatable.h"
#include <memory>

class CRecvProxyData;
using RecvVarProxyFn_t = void(__cdecl*)(const CRecvProxyData*, void*, void*);
using ArrayLengthProxyFn_t = void(__cdecl*)(void*, int, int);
using DataTableProxyFn_t = void(__cdecl*)(const RecvProp_t*, void**, void*, int);

class CRecvPropHook
{
public:
    CRecvPropHook( RecvProp_t* pRecvProp, const RecvVarProxyFn pNewProxyFn ) :
        pRecvProp( pRecvProp ), pOriginalFn( pRecvProp->oProxyFn )
    {
        SetProxy( pNewProxyFn );
    }

    // Get
    /* replace with our function */
    void Replace( RecvProp_t* pRecvProp )
    {
        this->pRecvProp = pRecvProp;
        this->pOriginalFn = pRecvProp->oProxyFn;
    }

    /* restore original function */
    void Restore( ) const
    {
        if ( this->pOriginalFn != nullptr )
            this->pRecvProp->oProxyFn = this->pOriginalFn;
    }

    void SetProxy( const RecvVarProxyFn pNewProxyFn ) const
    {
        this->pRecvProp->oProxyFn = pNewProxyFn;
    }

    RecvVarProxyFn GetOriginal( ) const
    {
        return this->pOriginalFn;
    }

private:
    // Values
    /* in future that is being modified and replace the original prop */
    RecvProp_t* pRecvProp = nullptr;
    /* original proxy function to make able to restore it later */
    RecvVarProxyFn pOriginalFn = nullptr;
};

namespace p {

	bool Setup();
	void Destroy();

	/* @section: handlers */
	void BaseViewModelSequence(const CRecvProxyData* pData, void* pStruct, void* pOut);
    void TickbaseProxy(const CRecvProxyData* pData, void* pStruct, void* pOut);

	/* @section: managers */
    inline std::shared_ptr<CRecvPropHook> hkBaseViewModelSequence;
    inline std::shared_ptr<CRecvPropHook> hkTickbaseProxy;
}