#pragma once
#include "../Classes/CModelInfo.h"
#include "../Classes/CStudio.h"
#include "../interfaces.h"
enum
{
	STUDIORENDER_DRAW_ENTIRE_MODEL = 0,
	STUDIORENDER_DRAW_OPAQUE_ONLY = 0x01,
	STUDIORENDER_DRAW_TRANSLUCENT_ONLY = 0x02,
	STUDIORENDER_DRAW_GROUP_MASK = 0x03,
	STUDIORENDER_DRAW_NO_FLEXES = 0x04,
	STUDIORENDER_DRAW_STATIC_LIGHTING = 0x08,
	STUDIORENDER_DRAW_ACCURATETIME = 0x10,
	STUDIORENDER_DRAW_NO_SHADOWS = 0x20,
	STUDIORENDER_DRAW_GET_PERF_STATS = 0x40,
	STUDIORENDER_DRAW_WIREFRAME = 0x80,
	STUDIORENDER_DRAW_ITEM_BLINK = 0x100,
	STUDIORENDER_SHADOWDEPTHTEXTURE = 0x200,
	STUDIORENDER_SSAODEPTHTEXTURE = 0x1000,
	STUDIORENDER_GENERATE_STATS = 0x8000,
};

enum EOverrideType : int
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SSAO_DEPTH_WRITE
};

struct MaterialLightingState_t
{
	Vector			vecAmbientCube[6];
	Vector			vecLightingOrigin;
	int				nLocalLightCount;
	//LightDesc_t		localLightDesc[4];
};

struct DrawModelResults_t;
struct ColorMeshInfo_t;
struct StudioDecalHandle_t { int iUnused; };
struct DrawModelInfo_t
{
	studiohdr_t* pStudioHdr;
	studiohwdata_t* pHardwareData;
	StudioDecalHandle_t		hDecals;
	int						iSkin;
	int						iBody;
	int						iHitboxSet;
	IClientRenderable* pClientEntity;
	int						iLOD;
	ColorMeshInfo_t* pColorMeshes;
	bool					bStaticLighting;
	MaterialLightingState_t	lightingState;
};


class IStudioRender {

public:

	void SetColorModulation(const float* arrColor)
	{
		util::CallVFunc<void>(this, 27U, arrColor);
	}

	void SetAlphaModulation(float flAlpha)
	{
		util::CallVFunc<void>(this, 28U, flAlpha);
	}

	void DrawModel(DrawModelResults_t* pResults, const DrawModelInfo_t& info, matrix3x4a_t* pBoneToWorld, float* flFlexWeights, float* flFlexDelayedWeights, const Vector& vecModelOrigin, int nFlags)
	{
		util::CallVFunc<void>(this, 29U, pResults, &info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, &vecModelOrigin, nFlags);
	}

	void ForcedMaterialOverride(IMaterial* pMaterial, EOverrideType nOverrideType = OVERRIDE_NORMAL, int nOverrides = 0)
	{
		util::CallVFunc<void>(this, 33U, pMaterial, nOverrideType, nOverrides);
	}

	bool IsForcedMaterialOverride() {
		return util::CallVFunc<bool>(this, 34U);
	}

	//void SetColorModulation(float const* arrColor) {
	//	util::util::CallVFunc<void>(this, 27, arrColor);
	//}

	void SetColorModulation(float const* arrColor, IMaterial* material) {

		if (material) {

			material->ColorModulate(arrColor[0], arrColor[1], arrColor[2]);

			bool bFound = false;
			auto tint = material->FindVar(("$envmaptint"), &bFound);
			if (bFound)
				tint->SetVector(arrColor[0], arrColor[1], arrColor[2]);
		}

		SetColorModulation(arrColor);
	}

	//void SetAlphaModulation(float flAlpha) {
	//	util::util::CallVFunc<void>(this, 28, flAlpha);
	//}

	//void ForcedMaterialOverride(IMaterial* pMaterial, EOverrideType nOverrideType = OVERRIDE_NORMAL, int nOverrides = 0) {
	//	util::util::CallVFunc<void>(this, 33, pMaterial, nOverrideType, nOverrides);
	//}

	//bool IsForcedMaterialOverride() {
	//	return util::util::CallVFunc<bool>(this, 34);
	//}
};