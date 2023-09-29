#pragma once

enum RenderableModelType_t
{
	RENDERABLE_MODEL_UNKNOWN_TYPE = -1,
	RENDERABLE_MODEL_ENTITY = 0,
	RENDERABLE_MODEL_STUDIOMDL,
	RENDERABLE_MODEL_STATIC_PROP,
	RENDERABLE_MODEL_BRUSH,
};


class IClientLeafSystem {

public:

	virtual void CreateRenderableHandle(IClientRenderable* pRenderable, bool bRenderWithViewModels, int nType, RenderableModelType_t nModelType, uint32_t nSplitscreenEnabled = 0xFFFFFFFF) = 0; // = RENDERABLE_MODEL_UNKNOWN_TYPE ) = 0;
	virtual void RemoveRenderable(uint32_t handle) = 0;
	virtual void AddRenderableToLeaves(uint32_t renderable, int nLeafCount, unsigned short* pLeaves) = 0;
	virtual void SetTranslucencyType(uint32_t handle, int nType) = 0;
	virtual void RenderInFastReflections(uint32_t handle, bool bRenderInFastReflections) = 0;
	virtual void DisableShadowDepthRendering(uint32_t handle, bool bDisable) = 0;
	virtual void DisableCSMRendering(uint32_t handle, bool bDisable) = 0;


	//void CreateRenderableHandle(IClientRenderable* pObject) {

	//	typedef void(__thiscall* fn)(void*, int, int, char, signed int, char);
	//	util::CallVFunc<fn>(this, 0)(this, reinterpret_cast<uintptr_t>(pObject), 0, 0, 0xFFFFFFFF, 0xFFFFFFFF);
	//}

	//void AddRenderable(IClientRenderable* pObject) {

	//	typedef void(__thiscall* fn)(void*, int, int, int, int, int);
	//	util::CallVFunc<fn>(this, 7)(this, reinterpret_cast<uintptr_t>(pObject), 0, 0, 0xFFFFFFFF, 0xFFFFFFFF);
	//}

private:


};