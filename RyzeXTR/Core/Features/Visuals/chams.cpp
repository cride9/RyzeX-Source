#include "chams.h"
#include "../../Interface/Classes/CKeyValues.h"
#include "../Rage/Animations/LocalAnimation.h"
#include "../../SDK/math.h"
#include "../Misc/misc.h"
#include "../Rage/Animations/Lagcompensation.h"
#include "../../SDK/InputSystem.h"
#include "../../SDK/Menu/gui.h"
#include "../Misc/Playerlist.h"

#include "drawlist.h"

static std::string_view szScrollProxies = XorStr(R"#(
	"texturescroll"
    {
        "texturescrollvar" "$basetexturetransform"
        "texturescrollrate" "0.4"
        "texturescrollangle" "90"
    }
)#");

IMaterial* RyzeCreateMaterial(std::string_view szName, std::string_view szShader, const char* szMaterial) {
	

	CKeyValues* pKeyValues = new CKeyValues(szShader.data());
	pKeyValues->LoadFromBuffer(szName.data(), szMaterial);

	IMaterial* pReturnValue = i::MaterialSystem->CreateMaterial(szName.data(), pKeyValues);

	return pReturnValue;
}

IMaterial* chams::CreateMaterial(std::string_view szName, std::string_view szShader, std::string_view szBaseTexture, std::string_view szEnvMap, bool bIgnorez, bool bWireframe, std::string_view szProxies)
{
	/*
	 * @note: materials info:
	 1	shaders: https://developer.valvesoftware.com/wiki/Category:Shaders
	 -		parameters: https://developer.valvesoftware.com/wiki/Category:List_of_Shader_Parameters
	 2	proxies: https://developer.valvesoftware.com/wiki/Material_proxies
	 -		list: https://developer.valvesoftware.com/wiki/List_Of_Material_Proxies
	 *
	 * use "mat_texture_list 1" command to see full materials list
	 */

	const std::string szMaterial = std::vformat(R"#("{0}"
	{{
		"$basetexture"		"{1}"
		"$envmap"			"{2}"
		"$envmapfresnel"	"0"
		"$model"			"1"
		"$translucent"		"0"
		"$ignorez"			"{3}"
		"$selfillum"		"1"
		"$halflambert"		"1"
		"$wireframe"		"{4}"
		"$nofog"			"1"
		"proxies"
		{{
			{5}
		}}
	}})#", std::make_format_args(szShader, szBaseTexture, szEnvMap, bIgnorez ? 1 : 0, bWireframe ? 1 : 0, szProxies));



	// load to memory
	CKeyValues* pKeyValues = new CKeyValues(szShader.data());
	pKeyValues->LoadFromBuffer(szName.data(), szMaterial.c_str());

	// create from buffer
	IMaterial* pReturn = i::MaterialSystem->CreateMaterial(szName.data(), pKeyValues);

	return pReturn;
}

void MatrixSetOrigin( Vector pos, matrix3x4a_t& matrix )
{
	matrix[ 0 ][ 3 ] = pos.x;
	matrix[ 1 ][ 3 ] = pos.y;
	matrix[ 2 ][ 3 ] = pos.z;
}

bool GenerateLerpedMatrix(CBaseEntity* pEntity, matrix3x4a_t* out)
{
	if (!pEntity)
		return false;

	auto& pLog = lagcomp.GetLog(pEntity->EntIndex());
	if (!&pLog || !pLog.pRecord.size() || pLog.iLastValid + 1 >= pLog.pRecord.size())
		return false;

	const auto& FirstInvalid = &pLog.pRecord[pLog.iLastValid];
	const auto& LastInvalid = &pLog.pRecord[pLog.iLastValid + 1];

	if (FirstInvalid->bDormant)
		return false;

	if (LastInvalid->flSimulationTime - FirstInvalid->flSimulationTime > 0.5f)
		return false;

	if (!LastInvalid->bDidShot && (!LastInvalid->vecOrigin.DistTo(FirstInvalid->vecOrigin) || LastInvalid->flDuck == FirstInvalid->flDuck && LastInvalid->vecEyeAngles == FirstInvalid->vecEyeAngles && LastInvalid->vecOrigin == FirstInvalid->vecOrigin))
		return false;

	const auto NextOrigin = LastInvalid->vecOrigin;
	const auto curtime = i::GlobalVars->flCurrentTime;

	auto flDelta = 1.f - (curtime - LastInvalid->flInterpTime) / (LastInvalid->flSimulationTime - FirstInvalid->flSimulationTime);
	if (flDelta < 0.f || flDelta > 1.f)
		LastInvalid->flInterpTime = curtime;

	flDelta = 1.f - (curtime - LastInvalid->flInterpTime) / (LastInvalid->flSimulationTime - FirstInvalid->flSimulationTime);

	flDelta = std::clamp( flDelta, 0.f, 1.f );

	const auto lerp = M::Lerp(NextOrigin, FirstInvalid->vecOrigin, flDelta );

	matrix3x4a_t ret[256];
	memcpy(ret, FirstInvalid->pMatricies[VISUAL], sizeof(matrix3x4a_t) * 256);

	for (size_t i{ }; i < 256; ++i) {
		const auto matrix_delta = Vector( FirstInvalid->pMatricies[VISUAL][ i ][ 0 ][ 3 ], FirstInvalid->pMatricies[VISUAL][ i ][ 1 ][ 3 ], FirstInvalid->pMatricies[VISUAL][ i ][ 2 ][ 3 ] ) - FirstInvalid->vecOrigin;
		MatrixSetOrigin(matrix_delta + lerp, ret[i]);
	}

	memcpy(out, ret, sizeof(matrix3x4a_t[256]));
	return true;
	return false;
}

static void BeginChams( IMaterial* pMaterial, float const* flColor, bool bIgnoreZ, bool bWireFrame ) {

	if (pMaterial == nullptr)
		return;

	i::StudioRender->SetColorModulation( flColor, pMaterial );
	i::StudioRender->SetAlphaModulation( flColor[ 3 ] );

	pMaterial->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, bIgnoreZ );
	pMaterial->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, bWireFrame );

	i::StudioRender->ForcedMaterialOverride( pMaterial );
}

static void EndChams(bool shouldNullptr = true) {

	static float reset[ 3 ] = { 1, 1, 1 };

	i::StudioRender->SetColorModulation( reset );
	i::StudioRender->SetAlphaModulation( 1.f );
	if (shouldNullptr)
		i::StudioRender->ForcedMaterialOverride( nullptr );
}

static void chams::PrepareMaterial() {

	if (!materials[DEFAULT] || materials[DEFAULT]->IsErrorMaterial())
		materials[DEFAULT] = CreateMaterial("ryzextr_players", "VertexLitGeneric");
	else
		materials[DEFAULT]->IncrementReferenceCount();

	if (!materials[FLAT] || materials[FLAT]->IsErrorMaterial())
		materials[FLAT] = CreateMaterial("ryzextr_playersflat", "UnlitGeneric");
	else
		materials[FLAT]->IncrementReferenceCount();

	if (!materials[GLOW] || materials[GLOW]->IsErrorMaterial())
		materials[GLOW] = RyzeCreateMaterial("ryzextr_glow", "VertexLitGeneric", GlowChams);
	else
		materials[GLOW]->IncrementReferenceCount();

	if (!materials[THINGLOW] || materials[THINGLOW]->IsErrorMaterial())
		materials[THINGLOW] = i::MaterialSystem->FindMaterial("dev/glow_armsrace", nullptr, true, nullptr);
	else
		materials[THINGLOW]->IncrementReferenceCount();

	if (!materials[ANIMATED] || materials[ANIMATED]->IsErrorMaterial())
		materials[ANIMATED] = RyzeCreateMaterial("ryzextr_animated", "VertexLitGeneric", AnimatedChams); 
	else
		materials[ANIMATED]->IncrementReferenceCount();
}
using namespace cfg::model;
bool chams::DrawChams(CBaseEntity* pLocal, DrawModelResults_t* pResults, const DrawModelInfo_t& info, matrix3x4a_t* pBoneToWorld, float* flFlexWeights, float* flFlexDelayedWeights, const Vector& vecModelOrigin, int nFlags) {

	static auto original = detour::drawModel.GetOriginal<decltype(&h::hkDrawModel)>();

	if (info.pStudioHdr && strstr(info.pStudioHdr->szName, "prop")) {

		bool ret = false;
		if (bProps) {
			BeginChams(materials[iPropsType], flPropsColor, false, bPropsXhair);
			original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			ret = true;
		}
		else {
			EndChams(false);
			original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			ret = false;
		}
		if (bPropsOverlay) {
			BeginChams(materials[GLOW], flPropsOverlayColor, false, bPropsOverlayXhair);
			original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			ret = true;
		}
		if (bPropsThinOverlay) {
			BeginChams(materials[THINGLOW], flPropsThinOverlayColor, false, bPropsThinOverlayXhair);
			original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			ret = true;
		}
		if (bPropsAnimOverlay) {
			BeginChams(materials[ANIMATED], flPropsAnimOverlayColor, false, bPropsAnimOverlayXhair);
			original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			ret = true;
		}
		return ret;
	}

	if (nFlags & (STUDIO_RENDER | STUDIO_SKIP_FLEXES | STUDIO_DONOTMODIFYSTENCILSTATE | STUDIO_NOLIGHTING_OR_CUBEMAP | STUDIO_SKIP_DECALS))
		return false;

	IClientRenderable* pRenderable = info.pClientEntity;

	if (!pRenderable)
		return false;

	CBaseEntity* pEnt = pRenderable->GetIClientUnknown()->GetBaseEntity();

	if (!pEnt || !g::pLocal)
		return false;

	const std::string_view& szModelName = info.pStudioHdr->szName;

	PrepareMaterial();
	for (auto pCurrent : materials)
		if (pCurrent->IsErrorMaterial())
			return false;
	
	if (szModelName.find("player\\contactshadow") != std::string_view::npos) {
		static float reset[4] = { 0, 0, 0, 0 };
		BeginChams(materials[0], reset, false, false);
		//original(i::StudioRender, 0, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
		original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
		return true;
	}

	bool bRagdoll = (szModelName.find("player") != std::string_view::npos && !pEnt->IsAlive());
	if ((pEnt->IsPlayer() && pEnt->IsAlive()) || bRagdoll) {

		if (pEnt == g::pLocal) {

			if (cfg::misc::iThirdPersonDistance == 0) {

				i::StudioRender->SetAlphaModulation(0.f);
				original(i::StudioRender, 0, pResults, info, localAnim->GetRealMatrix().data(), flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				//original(i::StudioRender, 0, pResults, info, g_LocalAnimations->GetRealMatrix().data(), flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				return true;
			}

			matrix3x4a_t* desyncMatrix = localAnim->GetDesyncMatrix().data();

			if (bDesyncChams) {

				if (!bChams[2]) {
					EndChams();
					if (cfg::model::bBlend && pEnt->IsScoped())
						i::StudioRender->SetAlphaModulation(cfg::model::flBlend * 0.01f); 
					original(i::StudioRender, 0, pResults, info, localAnim->GetRealMatrix().data(), flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				}

				if (desyncMatrix && !desyncMatrix->GetOrigin().IsZero()) {
					BeginChams(materials[bDesyncType], flDesyncChamsCol, false, bDesyncChamsXhair);
					original(i::StudioRender, 0, pResults, info, localAnim->GetDesyncMatrix().data(), flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				}
			}
			if (bChams[2]) {

				if (bChamsXQZ[2]) {
					BeginChams(materials[iType[2]], ChamsColorXQZ[2], true, bXhairXQZ[2]);
					if (cfg::model::bBlend && pEnt->IsScoped())
						i::StudioRender->SetAlphaModulation(cfg::model::flBlend * 0.01f);
					original(i::StudioRender, 0, pResults, info, localAnim->GetRealMatrix().data(), flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				}
				BeginChams(materials[iType[2]], ChamsColor[2], false, bXhair[2]);
				if (cfg::model::bBlend && pEnt->IsScoped())
					i::StudioRender->SetAlphaModulation(cfg::model::flBlend * 0.01f);
				original(i::StudioRender, 0, pResults, info, localAnim->GetRealMatrix().data(), flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			else {

				EndChams();
				if (cfg::model::bBlend && pEnt->IsScoped())
					i::StudioRender->SetAlphaModulation(cfg::model::flBlend * 0.01f);
				original(i::StudioRender, 0, pResults, info, localAnim->GetRealMatrix().data(), flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bOverlay[2]) {

				if (bOverlayXQZ[2]) {
					BeginChams(materials[GLOW], OverlayColor[2], true, bOverlayXhairXQZ[2]);
					original(i::StudioRender, 0, pResults, info, localAnim->GetRealMatrix().data(), flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				}
				BeginChams(materials[GLOW], OverlayColor[2], false, bOverlayXhairXQZ[2]);
				original(i::StudioRender, 0, pResults, info, localAnim->GetRealMatrix().data(), flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bThinOverlay[2]) {

				if (bThinOverlayXQZ[2]) {
					BeginChams(materials[THINGLOW], ThinOverlayColor[2], true, bThinOverlayXhairXQZ[2]);
					original(i::StudioRender, 0, pResults, info, localAnim->GetRealMatrix().data(), flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				}
				BeginChams(materials[THINGLOW], ThinOverlayColor[2], false, bThinOverlayXhairXQZ[2]);
				original(i::StudioRender, 0, pResults, info, localAnim->GetRealMatrix().data(), flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bAnimOverlay[2]) {

				if (bAnimOverlayXQZ[2]) {
					BeginChams(materials[ANIMATED], AnimOverlayColor[2], true, bAnimOverlayXhairXQZ[2]);
					original(i::StudioRender, 0, pResults, info, localAnim->GetRealMatrix().data(), flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				}
				BeginChams(materials[ANIMATED], AnimOverlayColor[2], false, bAnimOverlayXhairXQZ[2]);
				original(i::StudioRender, 0, pResults, info, localAnim->GetRealMatrix().data(), flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			return true;
		}
		else if (!pEnt->IsEnemy(g::pLocal)) {

			if (bChams[1]) {

				if (bChamsXQZ[1]) {
					BeginChams(materials[iType[1]], ChamsColorXQZ[1], true, bXhairXQZ[1]);
					original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				}
				BeginChams(materials[iType[1]], ChamsColor[1], false, bXhair[1]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			else {

				EndChams();
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bOverlay[1]) {

				if (bOverlayXQZ[1]) {
					BeginChams(materials[GLOW], OverlayColorXQZ[1], true, bOverlayXhairXQZ[1]);
					original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				}
				BeginChams(materials[GLOW], OverlayColor[1], false, bOverlayXhair[1]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bThinOverlay[1]) {

				if (bThinOverlayXQZ[1]) {
					BeginChams(materials[THINGLOW], ThinOverlayColorXQZ[1], true, bThinOverlayXhairXQZ[1]);
					original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				}
				BeginChams(materials[THINGLOW], ThinOverlayColor[1], false, bThinOverlayXhair[1]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bAnimOverlay[1]) {

				if (bAnimOverlayXQZ[1]) {
					BeginChams(materials[ANIMATED], AnimOverlayColorXQZ[1], true, bAnimOverlayXhairXQZ[1]);
					original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				}
				BeginChams(materials[ANIMATED], AnimOverlayColor[1], false, bAnimOverlayXhair[1]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			return true;
		}
		else if (pEnt->IsEnemy(g::pLocal)) {

			//auto pLog = &lagcomp.GetLog( pEnt->EntIndex( ) );
			//if (pLog->pRecord.size( ) >= 2) 
			//{
			//	// left
			//	BeginChams( materials[ iBacktrackType ], Color( 255, 0, 0, 255 ).BaseAlpha().data(), true, bBacktrackXhair );
			//	original( i::StudioRender, 0U, pResults, info, pLog->pRecord.front( ).pMatricies[ LEFT ], flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags );

			//	// center
			//	BeginChams( materials[ iBacktrackType ], Color( 0, 0, 255, 255 ).BaseAlpha( ).data( ), true, bBacktrackXhair );
			//	original( i::StudioRender, 0U, pResults, info, pLog->pRecord.front( ).pMatricies[ CENTER ], flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags );

			//	// right
			//	BeginChams( materials[ iBacktrackType ], Color( 0, 255, 0, 255 ).BaseAlpha( ).data( ), true, bBacktrackXhair );
			//	original( i::StudioRender, 0U, pResults, info, pLog->pRecord.front().pMatricies[ RIGHT ], flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags );
			//}

			if (bBacktrackChams && !bRagdoll) {

				auto pLog = &lagcomp.GetLog(pEnt->EntIndex());
				if (pLog && pLog->pRecord.size() >= 2) {

					std::array<float, 4> flBacktrackColor = { cfg::model::flBacktrackColor[0], cfg::model::flBacktrackColor[1], cfg::model::flBacktrackColor[2], cfg::model::flBacktrackColor[3] };
					flBacktrackColor[3] *= min(pLog->pRecord.at(pLog->iLastValid).vecOrigin.DistTo(pEnt->GetVecOrigin()) / 10.f, 1.f);

					BeginChams(materials[iBacktrackType], flBacktrackColor.data(), true, bBacktrackXhair);
					original(i::StudioRender, 0U, pResults, info, pLog->pRecord.at(pLog->iLastValid).pMatricies[VISUAL], flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				}
			}
			if (bChams[0]) {

				if (bChamsXQZ[0]) {
					BeginChams(materials[iType[0]], ChamsColorXQZ[0], true, bXhairXQZ[0]);
					original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				}
				BeginChams(materials[iType[0]], ChamsColor[0], false, bXhair[0]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			else {

				EndChams();
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bOverlay[0]) {

				if (bOverlayXQZ[0]) {
					BeginChams(materials[GLOW], OverlayColorXQZ[0], true, bOverlayXhairXQZ[0]);
					original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				}
				BeginChams(materials[GLOW], OverlayColor[0], false, bOverlayXhair[0]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bThinOverlay[0]) {

				if (bThinOverlayXQZ[0]) {
					BeginChams(materials[THINGLOW], ThinOverlayColorXQZ[0], true, bThinOverlayXhairXQZ[0]);
					original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				}
				BeginChams(materials[THINGLOW], ThinOverlayColor[0], false, bThinOverlayXhair[0]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bAnimOverlay[0]) {

				if (bAnimOverlayXQZ[0]) {
					BeginChams(materials[ANIMATED], AnimOverlayColorXQZ[0], true, bAnimOverlayXhairXQZ[0]);
					original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				}
				BeginChams(materials[ANIMATED], AnimOverlayColor[0], false, bAnimOverlayXhair[0]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			return true;
		}
	}
	//else if (szModelName.find("sleeve") != std::string_view::npos) {

	//	// get original sleeves material
	//	IMaterial* pSleeveMaterial = i::MaterialSystem->FindMaterial(szModelName.data(), TEXTURE_GROUP_MODEL);

	//	// check is valid material
	//	if (pSleeveMaterial == nullptr)
	//		return false;

	//}
	else if (szModelName.find("arms") != std::string_view::npos && !i::Input->bCameraInThirdPerson) {

		bool ret = false;
		if ((szModelName.find("sleeve") != std::string_view::npos) && bSleeve) {

			if (bSleeve) {
				BeginChams(materials[iSleeveType], flSleeveColor, false, bSleeveXhair);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				ret = true;
			}
			else {
				EndChams(false);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				ret = false;
			}
			if (bSleeveOverlay) {
				BeginChams(materials[GLOW], flSleeveOverlayColor, false, bSleeveOverlayXhair);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				ret = true;
			}
			if (bSleeveThinOverlay) {
				BeginChams(materials[THINGLOW], flSleeveThinOverlayColor, false, bSleeveThinOverlayXhair);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				ret = true;
			}
			if (bSleeveAnimOverlay) {
				BeginChams(materials[ANIMATED], flSleeveAnimOverlayColor, false, bSleeveAnimOverlayXhair);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				ret = true;
			}
			return ret;
		}

		if (bViewmodel) {
			BeginChams(materials[iViewmodelType], flViewmodelColor, false, bViewmodelXhair);
			original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			ret = true;
		}
		else {
			EndChams(false);
			original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			ret = false;
		}
		if (bViewmodelOverlay) {
			BeginChams(materials[GLOW], flViewmodelOverlayColor, false, bViewmodelOverlayXhair);
			original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			ret = true;
		}
		if (bViewmodelThinOverlay) {
			BeginChams(materials[THINGLOW], flViewmodelThinOverlayColor, false, bViewmodelThinOverlayXhair);
			original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			ret = true;
		}
		if (bViewmodelAnimOverlay) {
			BeginChams(materials[ANIMATED], flViewmodelAnimOverlayColor, false, bViewmodelAnimOverlayXhair);
			original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			ret = true;
		}
		return ret;
	}
	else if (szModelName.find("weapons\\v_") != std::string_view::npos && !i::Input->bCameraInThirdPerson) {

		IMaterial* pViewModelMaterial = i::MaterialSystem->FindMaterial(szModelName.data(), TEXTURE_GROUP_MODEL);

		if (pViewModelMaterial == nullptr)
			return false;

		bool ret = false;
		if (bWeapon) {
			BeginChams(materials[iWeaponType], flWeaponColor, false, bWeaponXhair);
			original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			ret = true;
		}
		else {
			if (i::StudioRender->IsForcedMaterialOverride()) {
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				ret = false;
			}
			else {
				EndChams();
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				ret = false;
			}
		}
		if (bWeaponOverlay) {
			BeginChams(materials[GLOW], flWeaponOverlayColor, false, bWeaponOverlayXhair);
			original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			ret = true;
		}
		if (bWeaponThinOverlay) {
			BeginChams(materials[THINGLOW], bWeaponThinOverlayColor, false, bWeaponThinOverlayXhair);
			original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			ret = true;
		}
		if (bWeaponAnimOverlay) {
			BeginChams(materials[ANIMATED], flWeaponAnimOverlayColor, false, flWeaponAnimOverlayXhair);
			original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			ret = true;
		}
		return ret;
	}
	else if ((szModelName.find("weapons\\w_") != std::string_view::npos)) {

		CBaseEntity* pEntity = nullptr;
		for (size_t i = 1; i < i::GlobalVars->nMaxClients; i++) {

			if (playerList::arrPlayers[i].iIndex != i)
				continue;
			
			CBaseEntity* tempEntity = static_cast<CBaseEntity*>(i::EntityList->GetClientEntity(i));
			if (tempEntity && tempEntity->IsAlive()) {
				if ((vecModelOrigin - tempEntity->GetVecOrigin()).Length() < 100.f) {
					if (pEntity) {
						if ((vecModelOrigin - tempEntity->GetVecOrigin()).Length() < (vecModelOrigin - pEntity->GetVecOrigin()).Length())
							pEntity = tempEntity;
					}
					else {
						pEntity = tempEntity;
					}
				}
			}
		}
		if (!pEntity)
			return false;

		// enemy
		if (pEntity->IsEnemy(g::pLocal)) {

			if (bAttachment[ENEMY]) {

				BeginChams(chams::materials[iAttachmentMaterial[ENEMY]], flAttachmentColor[ENEMY], false, bAttachmentXhair[ENEMY]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			else {
				if (!i::StudioRender->IsForcedMaterialOverride())
					EndChams();
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bAttachmentOverlay[ENEMY]) {

				BeginChams(chams::materials[GLOW], bAttachmentOverlayColor[ENEMY], false, bAttachmentOverlayXhair[ENEMY]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bAttachmentThinOverlay[ENEMY]) {

				BeginChams(chams::materials[THINGLOW], bAttachmentThinOverlayColor[ENEMY], false, bAttachmentThinOverlayXhair[ENEMY]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bAttachmentAnimatedOverlay[ENEMY]) {

				BeginChams(chams::materials[ANIMATED], bAttachmentAnimatedOverlayColor[ENEMY], false, bAttachmentAnimatedOverlayXhair[ENEMY]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			return true;
		}

		// local
		else if (pEntity == g::pLocal) {

			if (cfg::misc::iThirdPersonDistance == 0) {
				i::StudioRender->SetAlphaModulation(0.f);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
				return true;
			}

			if (bAttachment[LOCAL]) {
				BeginChams(chams::materials[iAttachmentMaterial[LOCAL]], flAttachmentColor[LOCAL], true, bAttachmentXhair[LOCAL]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

				BeginChams(chams::materials[iAttachmentMaterial[LOCAL]], flAttachmentColor[LOCAL], false, bAttachmentXhair[LOCAL]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			else {
				if (!i::StudioRender->IsForcedMaterialOverride())
					EndChams();
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bAttachmentOverlay[LOCAL]) {

				BeginChams(chams::materials[GLOW], bAttachmentOverlayColor[LOCAL], false, bAttachmentOverlayXhair[LOCAL]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bAttachmentThinOverlay[LOCAL]) {

				BeginChams(chams::materials[THINGLOW], bAttachmentThinOverlayColor[LOCAL], false, bAttachmentThinOverlayXhair[LOCAL]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bAttachmentAnimatedOverlay[LOCAL]) {

				BeginChams(chams::materials[ANIMATED], bAttachmentAnimatedOverlayColor[LOCAL], false, bAttachmentAnimatedOverlayXhair[LOCAL]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			return true;
		}
		// teammate
		else if (!pEntity->IsEnemy(g::pLocal) && g::pLocal != pEntity) {

			if (bAttachment[TEAM]) {
				BeginChams(chams::materials[iAttachmentMaterial[TEAM]], flAttachmentColor[TEAM], true, bAttachmentXhair[TEAM]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

				BeginChams(chams::materials[iAttachmentMaterial[TEAM]], flAttachmentColor[TEAM], false, bAttachmentXhair[TEAM]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			else {
				if (!i::StudioRender->IsForcedMaterialOverride())
					EndChams();
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bAttachmentOverlay[TEAM]) {

				BeginChams(chams::materials[GLOW], bAttachmentOverlayColor[TEAM], false, bAttachmentOverlayXhair[TEAM]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bAttachmentThinOverlay[TEAM]) {

				BeginChams(chams::materials[THINGLOW], bAttachmentThinOverlayColor[TEAM], false, bAttachmentThinOverlayXhair[TEAM]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			if (bAttachmentAnimatedOverlay[TEAM]) {

				BeginChams(chams::materials[ANIMATED], bAttachmentAnimatedOverlayColor[TEAM], false, bAttachmentAnimatedOverlayXhair[TEAM]);
				original(i::StudioRender, 0U, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}
			return true;
		}
	}

	return false;
}