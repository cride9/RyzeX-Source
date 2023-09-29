#pragma once
#include "../../SDK/Entity.h"
#include "../../Hooks/hooks.h"
#include "../../Interface/Interfaces/IMaterial.h"
#include "../../SDK/Enums.h"
#include "../../SDK/Menu/config.h"
#include <format>

constexpr static const char* CoolChams =

R"#("VertexLitGeneric"{
		  
	"$basetexture" "models/weapons/customization/paints/anodized_multi/smoke"
	"$nofog" "1"
	"$envmap" "env_cubemap"
	"$envmaptint" "[1 1 1]"
	"$phong" "1"
	"$basemapalphaphongmask" "1"
	"$phongboost" "0"
	"$rimlight" "1"
	"$phongtint" "[1 1 1]"
	"$rimlightexponent" "9999999"
	"$rimlightboost" "0"
	"$selfillum" "1"
	"$ignorez" "0"
 
	"Proxies"
	{
		"TextureScroll"
		{
			"textureScrollVar" "$basetexturetransform"
			"textureScrollRate" "0.08"
			"textureScrollAngle" "45"
		}
 
		"TextureScroll"
		{
			"textureScrollVar" "$bumptransform"
			"textureScrollRate" "0.01"
			"textureScrollAngle" "45"
		}   
	}
})#";

constexpr static const char* GlowChams =

R"#("VertexLitGeneric"
			{ 
				"$additive"					"1" 
				"$envmap"					"models/effects/cube_white" 
				"$envmaptint"				"[1 1 1]" 
				"$envmapfresnel"			"1" 
				"$envmapfresnelminmaxexp" 	"[0 1 2]" 
				"$alpha" 					"1" 
			}
		)#";

constexpr static const char* AnimatedChams =

R"#("VertexLitGeneric"
			{ 
                "$basetexture" "models/inventory_items/dreamhack_trophies/dreamhack_star_blur"
                "$wireframe" "0"
                "$alpha" "10"
                "$additive" "1"
                "proxies"
                 {
                    "texturescroll"
                    {
                        "texturescrollvar" "$basetexturetransform"
                        "texturescrollrate" "0.4"
                        "texturescrollangle" "90"
                    }
                }
			}
		)#";

constexpr static const char* FlatChams =

R"#("UnlitGeneric"
		{
		 "$basetexture" "vgui/white_additive"
		  "$no_fullbright" "0"
		 "$ignorez"      "1"
		 "$envmap"       "env_cubemap"
		 "$nofog"        "1"
		"$model"        "1"
		"$nocull"       "0"
		"$selfillum"    "1"
		 "$halflambert"  "1"
		 "$znearer"      "0"
		 "$flat"         "0"
		} )#";

constexpr static const char* MaterialChams =

R"#("VertexLitGeneric"
		{
		 "$basetexture" "vgui/white_additive"
		 "$ignorez"      "0"
		 "$envmap"       ""
		 "$nofog"        "1"
		 "$model"        "1"
		 "$nocull"       "0"
		 "$selfillum"    "1"
		 "$halflambert"  "1"
		 "$znearer"      "0"
		 "$flat"         "1"
		 "$reflectivity" "[1 1 1]"
		} )#";

enum EMATERIAL : int {

	DEFAULT,
	FLAT,
	GLOW,
	THINGLOW,
	ANIMATED,
	MATERIAL_MAX
};

namespace chams {

	IMaterial* CreateMaterial(std::string_view szName, std::string_view szShader, std::string_view szBaseTexture = "vgui/white", std::string_view szEnvMap = "", bool bIgnorez = false, bool bWireframe = false, std::string_view szProxies = "");
	bool DrawChams(CBaseEntity*, DrawModelResults_t*, const DrawModelInfo_t&, matrix3x4a_t*, float*, float*, const Vector&, int);
	bool DrawChamsMDL(IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
	void PrepareMaterial();

	inline std::array<IMaterial*, 5> materials;
}