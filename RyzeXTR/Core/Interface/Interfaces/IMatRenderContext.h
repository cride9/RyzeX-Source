#pragma once
#include "../Classes/CTexture.h"
enum ELightType : int
{
	MATERIAL_LIGHT_DISABLE = 0,
	MATERIAL_LIGHT_POINT,
	MATERIAL_LIGHT_DIRECTIONAL,
	MATERIAL_LIGHT_SPOT
};

enum ELightTypeOptimizationFlags
{
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION0 = 1,
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION1 = 2,
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION2 = 4,
	LIGHTTYPE_OPTIMIZATIONFLAGS_DERIVED_VALUES_CALCED = 8
};

struct LightDesc_t
{
	void InitDirectional(const Vector& vecDirection, const Vector& vecColor)
	{
		this->nType = MATERIAL_LIGHT_DIRECTIONAL;
		this->vecColor = vecColor;
		this->vecDirection = vecDirection;
		this->flRange = 0.0f;
		this->flAttenuation0 = 1.0f;
		this->flAttenuation1 = 0.f;
		this->flAttenuation2 = 0.f;

		iFlags = LIGHTTYPE_OPTIMIZATIONFLAGS_DERIVED_VALUES_CALCED;

		if (flAttenuation0)
			iFlags |= LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION0;
		if (flAttenuation1)
			iFlags |= LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION1;
		if (flAttenuation2)
			iFlags |= LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION2;

		if (nType == MATERIAL_LIGHT_SPOT)
		{
			flThetaDot = std::cosf(flTheta);
			flPhiDot = std::cosf(flPhi);

			if (const float flSpread = flThetaDot - flPhiDot; flSpread > 1.0e-10f)
				// note - this quantity is very sensitive to round off error. the sse reciprocal approximation won't cut it here.
				flOneOverThetaDotMinusPhiDot = 1.0f / flSpread;
			else
				// hard falloff instead of divide by zero
				flOneOverThetaDotMinusPhiDot = 1.0f;
		}
		else if (nType == MATERIAL_LIGHT_DIRECTIONAL)
		{
			// set position to be real far away in the right direction
			vecPosition = vecDirection;
			vecPosition *= 2.0e6;
		}

		flRangeSquared = flRange * flRange;
	}

	ELightType	nType;
	Vector		vecColor;
	Vector		vecPosition;
	Vector		vecDirection;
	float		flRange;
	float		flFalloff;
	float		flAttenuation0;
	float		flAttenuation1;
	float		flAttenuation2;
	float		flTheta;
	float		flPhi;
	float		flThetaDot;
	float		flPhiDot;
	float		flOneOverThetaDotMinusPhiDot;
	std::uint32_t iFlags;
protected:
	float		flRangeSquared;
};

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/client/glow_outline_effect.cpp
enum EStencilOperation : int
{
	STENCILOPERATION_KEEP = 1,
	STENCILOPERATION_ZERO = 2,
	STENCILOPERATION_REPLACE = 3,
	STENCILOPERATION_INCRSAT = 4,
	STENCILOPERATION_DECRSAT = 5,
	STENCILOPERATION_INVERT = 6,
	STENCILOPERATION_INCR = 7,
	STENCILOPERATION_DECR = 8,
	STENCILOPERATION_FORCE_DWORD = 0x7FFFFFFF
};

enum EStencilComparisonFunction : int
{
	STENCILCOMPARISONFUNCTION_NEVER = 1,
	STENCILCOMPARISONFUNCTION_LESS = 2,
	STENCILCOMPARISONFUNCTION_EQUAL = 3,
	STENCILCOMPARISONFUNCTION_LESSEQUAL = 4,
	STENCILCOMPARISONFUNCTION_GREATER = 5,
	STENCILCOMPARISONFUNCTION_NOTEQUAL = 6,
	STENCILCOMPARISONFUNCTION_GREATEREQUAL = 7,
	STENCILCOMPARISONFUNCTION_ALWAYS = 8,
	STENCILCOMPARISONFUNCTION_FORCE_DWORD = 0x7FFFFFFF
};

struct ShaderStencilState_t
{
	ShaderStencilState_t()
	{
		bEnable = false;
		PassOperation = FailOperation = ZFailOperation = STENCILOPERATION_KEEP;
		CompareFunction = STENCILCOMPARISONFUNCTION_ALWAYS;
		nReferenceValue = 0;
		uTestMask = uWriteMask = 0xFFFFFFFF;
	}

	bool						bEnable;
	EStencilOperation			FailOperation;
	EStencilOperation			ZFailOperation;
	EStencilOperation			PassOperation;
	EStencilComparisonFunction	CompareFunction;
	int							nReferenceValue;
	std::uint32_t				uTestMask;
	std::uint32_t				uWriteMask;
};

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h

class IRefCounted
{
public:
	virtual int AddReference() = 0;
	virtual int Release() = 0;
};
class IMatRenderContext : public IRefCounted
{
public:
	void BeginRender()
	{
		util::CallVFunc<void>(this, 2);
	}

	void EndRender()
	{
		util::CallVFunc<void>(this, 3);
	}

	void BindLocalCubemap(ITexture* pTexture)
	{
		util::CallVFunc<void>(this, 5, pTexture);
	}

	void SetRenderTarget(ITexture* pTexture)
	{
		util::CallVFunc<void>(this, 6, pTexture);
	}

	ITexture* GetRenderTarget()
	{
		return util::CallVFunc<ITexture*>(this, 7);
	}

	void ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false)
	{
		util::CallVFunc<void>(this, 12, bClearColor, bClearDepth, bClearStencil);
	}

	void SetLights(int nCount, const LightDesc_t* pLights)
	{
		util::CallVFunc<void>(this, 17, nCount, pLights);
	}

	void SetAmbientLightCube(Vector4D vecCube[6])
	{
		util::CallVFunc<void>(this, 18, vecCube);
	}

	void Viewport(int x, int y, int iWidth, int iHeight)
	{
		util::CallVFunc<void>(this, 40, x, y, iWidth, iHeight);
	}

	void GetViewport(int& x, int& y, int& iWidth, int& iHeight)
	{
		util::CallVFunc<void>(this, 41, std::ref(x), std::ref(y), std::ref(iWidth), std::ref(iHeight));
	}

	void ClearColor3ub(unsigned char r, unsigned char g, unsigned char b)
	{
		util::CallVFunc<void>(this, 78, r, g, b);
	}

	void ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		util::CallVFunc<void>(this, 79, r, g, b, a);
	}

	void DrawScreenSpaceRectangle(IMaterial* pMaterial, int iDestX, int iDestY, int iWidth, int iHeight, float flTextureX0, float flTextureY0, float flTextureX1, float flTextureY1, int iTextureWidth, int iTextureHeight, void* pClientRenderable = nullptr, int nXDice = 1, int nYDice = 1)
	{
		util::CallVFunc<void>(this, 114, pMaterial, iDestX, iDestY, iWidth, iHeight, flTextureX0, flTextureY0, flTextureX1, flTextureY1, iTextureWidth, iTextureHeight, pClientRenderable, nXDice, nYDice);
	}

	void PushRenderTargetAndViewport()
	{
		util::CallVFunc<void>(this, 119);
	}

	void PopRenderTargetAndViewport()
	{
		util::CallVFunc<void>(this, 120);
	}

	void SetLightingOrigin(/*Vector vecLightingOrigin*/float x, float y, float z)
	{
		util::CallVFunc<void>(this, 158, x, y, z);
	}

	void SetScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor = true)
	{
		util::CallVFunc<void>(this, 159, nLeft, nTop, nRight, nBottom, bEnableScissor);
	}

	void PopScissorRect()
	{
		util::CallVFunc<void>(this, 160);
	}
};

class CRefCounted
{
public:
	virtual void Destructor(char bDelete) = 0;
	virtual bool OnFinalRelease() = 0;

	void Release()
	{
		if (InterlockedDecrement(&vlRefCount) == 0 && OnFinalRelease())
			Destructor(1);
	}

private:
	volatile long vlRefCount;
};

template <class T>
class CBaseAutoPtr
{
public:
	CBaseAutoPtr() :
		pObject(nullptr) { }

	CBaseAutoPtr(T* pObject) :
		pObject(pObject) { }

	operator const void* () const
	{
		return pObject;
	}

	operator void* () const
	{
		return pObject;
	}

	operator const T* () const
	{
		return pObject;
	}

	operator const T* ()
	{
		return pObject;
	}

	operator T* ()
	{
		return pObject;
	}

	int	operator=(int i)
	{
		pObject = nullptr;
		return 0;
	}

	T* operator=(T* pSecondObject)
	{
		pObject = pSecondObject;
		return pSecondObject;
	}

	bool operator!() const
	{
		return (!pObject);
	}

	bool operator==(const void* pSecondObject) const
	{
		return (pObject == pSecondObject);
	}

	bool operator!=(const void* pSecondObject) const
	{
		return (pObject != pSecondObject);
	}

	bool operator==(T* pSecondObject) const
	{
		return operator==(static_cast<void*>(pSecondObject));
	}

	bool operator!=(T* pSecondObject) const
	{
		return operator!=(static_cast<void*>(pSecondObject));
	}

	bool operator==(const CBaseAutoPtr<T>& pSecondPtr) const
	{
		return operator==(static_cast<const void*>(pSecondPtr));
	}

	bool operator!=(const CBaseAutoPtr<T>& pSecondPtr) const
	{
		return operator!=(static_cast<const void*>(pSecondPtr));
	}

	T* operator->()
	{
		return pObject;
	}

	T& operator*()
	{
		return *pObject;
	}

	T** operator&()
	{
		return &pObject;
	}

	const T* operator->() const
	{
		return pObject;
	}

	const T& operator*() const
	{
		return *pObject;
	}

	T* const* operator&() const
	{
		return &pObject;
	}

protected:
	CBaseAutoPtr(const CBaseAutoPtr<T>& pSecondPtr) :
		pObject(pSecondPtr.pObject) { }

	void operator=(const CBaseAutoPtr<T>& pSecondPtr)
	{
		pObject = pSecondPtr.pObject;
	}

	T* pObject;
};

template <class T>
class CRefPtr : public CBaseAutoPtr<T>
{
	typedef CBaseAutoPtr<T> CBaseClass;
public:
	CRefPtr() { }

	CRefPtr(T* pInit)
		: CBaseClass(pInit) { }

	CRefPtr(const CRefPtr<T>& pRefPtr)
		: CBaseClass(pRefPtr) { }

	~CRefPtr()
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->Release();
	}

	void operator=(const CRefPtr<T>& pSecondRefPtr)
	{
		CBaseClass::operator=(pSecondRefPtr);
	}

	int operator=(int i)
	{
		return CBaseClass::operator=(i);
	}

	T* operator=(T* pSecond)
	{
		return CBaseClass::operator=(pSecond);
	}

	operator bool() const
	{
		return !CBaseClass::operator!();
	}

	operator bool()
	{
		return !CBaseClass::operator!();
	}

	void SafeRelease()
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->Release();

		CBaseClass::pObject = nullptr;
	}

	void AssignAddReference(T* pFrom)
	{
		if (pFrom != nullptr)
			pFrom->AddReference();

		SafeRelease();
		CBaseClass::pObject = pFrom;
	}

	void AddReferenceAssignTo(T*& pTo)
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->AddReference();

		SafeRelease(pTo);
		pTo = CBaseClass::pObject;
	}
};
