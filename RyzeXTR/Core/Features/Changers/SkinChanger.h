#pragma once
#include "../../Interface/interfaces.h"
#include "../../SDK/Menu/config.h"
#include "../../SDK/Entity.h"
#include "../../globals.h"
#include <unordered_set>
namespace detail
{
	template <typename Type, Type OffsetBasis, Type Prime>
	struct size_dependant_data
	{
		using type = Type;
		constexpr static auto k_offset_basis = OffsetBasis;
		constexpr static auto k_prime = Prime;
	};

	template <size_t Bits>
	struct size_selector;

	template <>
	struct size_selector<32>
	{
		using type = size_dependant_data<std::uint32_t, 0x811c9dc5ul, 16777619ul>;
	};

	template <>
	struct size_selector<64>
	{
		using type = size_dependant_data<std::uint64_t, 0xcbf29ce484222325ull, 1099511628211ull>;
	};

	// Implements FNV-1a hash algorithm
	template <std::size_t Size>
	class fnv_hash
	{
	private:
		using data_t = typename size_selector<Size>::type;

	public:
		using hash = typename data_t::type;

	private:
		constexpr static auto k_offset_basis = data_t::k_offset_basis;
		constexpr static auto k_prime = data_t::k_prime;

	public:
		template <std::size_t N>
		static __forceinline constexpr auto hash_constexpr(const char(&str)[N], const std::size_t size = N) -> hash
		{
			return static_cast<hash>(1ull * (size == 1
				? (k_offset_basis ^ str[0])
				: (hash_constexpr(str, size - 1) ^ str[size - 1])) * k_prime);
		}

		static auto __forceinline hash_runtime(const char* str) -> hash
		{
			auto result = k_offset_basis;
			do
			{
				result ^= *str++;
				result *= k_prime;
			} while (*(str - 1) != '\0');

			return result;
		}
	};
}

struct EConItem_t
{
	struct EconomyItem_t
	{
		EconomyItem_t(int entity_quality_index, int paint_kit_index, int stat_trak, float wear)
			: entity_quality_index(entity_quality_index), paint_kit_index(paint_kit_index), stat_trak(stat_trak), wear(wear) { }
		int entity_quality_index = 0;
		int paint_kit_index = 0;
		int stat_trak = 0;
		float wear = FLT_MIN;
	};
};

struct SkinObject_t
{
	SkinObject_t(const char* szName, const char* szModel, const char* szKillIcon = nullptr)
		: szName(szName), szModel(szModel), szKillIcon(szKillIcon) { }

	const char* szName = nullptr;
	const char* szModel = nullptr;
	const char* szKillIcon = nullptr;
};

struct AgentObject_t
{
	AgentObject_t(std::string szModelName, std::string szDisplayName)
		: szModelName(szModelName), szDisplayName(szDisplayName) {}

	std::string szModelName = "";
	std::string szDisplayName = "";
};


struct WeaponInfo_t
{
	constexpr WeaponInfo_t(const char* model, const char* icon = nullptr) :
		model(model),
		icon(icon)
	{}

	const char* model;
	const char* icon;
};

const std::unordered_map<int, const char*> mapGloveList =
{
	{ GLOVE_STUDDED_BLOODHOUND, "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl" },
	{ GLOVE_SPORTY, "models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl" },
	{ GLOVE_SLICK, "models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl" },
	{ GLOVE_LEATHER_HANDWRAPS, "models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl" },
	{ GLOVE_MOTORCYCLE, "models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl" },
	{ GLOVE_SPECIALIST, "models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl" },
	{ GLOVE_STUDDED_HYDRA, "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_hydra.mdl" },
	{ GLOVE_STUDDED_BROKENFANG, "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_brokenfang.mdl" }
};

// @note: u can find viewmodel indexes with "sv_precacheinfo"
const std::unordered_map<int, SkinObject_t> mapItemList =
{
	{ WEAPON_DEAGLE, { "Desert Eagle", "models/weapons/v_pist_deagle.mdl", "deagle" } },
	{ WEAPON_ELITE, { "Dual Berettas", "models/weapons/v_pist_elite.mdl", "elite" } },
	{ WEAPON_FIVESEVEN, { "Five-SeveN", "models/weapons/v_pist_fiveseven.mdl", "fiveseven" } },
	{ WEAPON_GLOCK, { "Glock-18", "models/weapons/v_pist_glock18.mdl", "glock" } },
	{ WEAPON_AK47, { "AK-47", "models/weapons/v_rif_ak47.mdl", "ak47" } },
	{ WEAPON_AUG, { "AUG", "models/weapons/v_rif_aug.mdl", "aug" } },
	{ WEAPON_AWP, { "AWP", "models/weapons/v_snip_awp.mdl", "awp" } },
	{ WEAPON_FAMAS, { "FAMAS", "models/weapons/v_rif_famas.mdl", "famas" } },
	{ WEAPON_G3SG1, { "G3SG1", "models/weapons/v_snip_g3sg1.mdl", "g3sg1" } },
	{ WEAPON_GALILAR, { "Galil AR", "models/weapons/v_rif_galilar.mdl", "galilar" } },
	{ WEAPON_M249, { "M249", "models/weapons/v_mach_m249para.mdl", "m249" } },
	{ WEAPON_M4A1, { "M4A4", "models/weapons/v_rif_m4a1.mdl", "m4a1" } },
	{ WEAPON_MAC10, { "MAC-10", "models/weapons/v_smg_mac10.mdl", "mac10" } },
	{ WEAPON_P90, { "P90", "models/weapons/v_smg_p90.mdl", "p90" } },
	{ WEAPON_MP5SD, { "MP5-SD", "models/weapons/v_smg_mp5sd.mdl", "mp5sd" } },
	{ WEAPON_UMP45, { "UMP-45", "models/weapons/v_smg_ump45.mdl", "ump45" } },
	{ WEAPON_XM1014, { "XM1014", "models/weapons/v_shot_xm1014.mdl", "xm1014" } },
	{ WEAPON_BIZON, { "PP-Bizon", "models/weapons/v_smg_bizon.mdl", "bizon" } },
	{ WEAPON_MAG7, { "MAG-7", "models/weapons/v_shot_mag7.mdl", "mag7" } },
	{ WEAPON_NEGEV, { "Negev", "models/weapons/v_mach_negev.mdl", "negev" } },
	{ WEAPON_SAWEDOFF, { "Sawed-Off", "models/weapons/v_shot_sawedoff.mdl", "sawedoff" } },
	{ WEAPON_TEC9, { "Tec-9", "models/weapons/v_pist_tec9.mdl", "tec9" } },
	{ WEAPON_HKP2000, { "P2000", "models/weapons/v_pist_hkp2000.mdl", "hkp2000" } },
	{ WEAPON_MP7, { "MP7", "models/weapons/v_smg_mp7.mdl", "mp7" } },
	{ WEAPON_MP9, { "MP9", "models/weapons/v_smg_mp9.mdl", "mp9" } },
	{ WEAPON_NOVA, { "Nova", "models/weapons/v_shot_nova.mdl", "nova" } },
	{ WEAPON_P250, { "P250", "models/weapons/v_pist_p250.mdl", "p250" } },
	{ WEAPON_SCAR20, { "SCAR-20", "models/weapons/v_snip_scar20.mdl", "scar20" } },
	{ WEAPON_SG556, { "SG 553", "models/weapons/v_rif_sg556.mdl", "sg556" } },
	{ WEAPON_SSG08, { "SSG 08", "models/weapons/v_snip_ssg08.mdl", "ssg08" } },
	{ WEAPON_KNIFE, { "Knife (Counter-Terrorists)", "models/weapons/v_knife_default_ct.mdl", "knife_default_ct" } },
	{ WEAPON_KNIFE_T, { "Knife (Terrorists)", "models/weapons/v_knife_default_t.mdl", "knife_t" } },
	{ WEAPON_M4A1_SILENCER, { "M4A1-S", "models/weapons/v_rif_m4a1_s.mdl", "m4a1_silencer" } },
	{ WEAPON_USP_SILENCER, { "USP-S", "models/weapons/v_pist_223.mdl", "usp_silencer" } },
	{ WEAPON_CZ75A, { "CZ75 Auto", "models/weapons/v_pist_cz_75.mdl", "cz75a" } },
	{ WEAPON_REVOLVER, { "R8 Revolver", "models/weapons/v_pist_revolver.mdl", "revolver" } },
	{ WEAPON_KNIFE_BAYONET, { "Bayonet", "models/weapons/v_knife_bayonet.mdl", "bayonet" } },
	{ WEAPON_KNIFE_FLIP, { "Flip Knife", "models/weapons/v_knife_flip.mdl", "knife_flip" } },
	{ WEAPON_KNIFE_GUT, { "Gut Knife", "models/weapons/v_knife_gut.mdl", "knife_gut" } },
	{ WEAPON_KNIFE_KARAMBIT, { "Karambit", "models/weapons/v_knife_karam.mdl", "knife_karambit" } },
	{ WEAPON_KNIFE_M9_BAYONET, { "M9 Bayonet", "models/weapons/v_knife_m9_bay.mdl", "knife_m9_bayonet" } },
	{ WEAPON_KNIFE_TACTICAL, { "Huntsman Knife", "models/weapons/v_knife_tactical.mdl", "knife_tactical" } },
	{ WEAPON_KNIFE_FALCHION, { "Falchion Knife", "models/weapons/v_knife_falchion_advanced.mdl", "knife_falchion" } },
	{ WEAPON_KNIFE_SURVIVAL_BOWIE, { "Bowie Knife", "models/weapons/v_knife_survival_bowie.mdl", "knife_survival_bowie" } },
	{ WEAPON_KNIFE_BUTTERFLY, { "Butterfly Knife", "models/weapons/v_knife_butterfly.mdl", "knife_butterfly" } },
	{ WEAPON_KNIFE_PUSH, { "Shadow Daggers", "models/weapons/v_knife_push.mdl", "knife_push" } },
	{ WEAPON_KNIFE_CORD, { "Paracord Knife", "models/weapons/v_knife_cord.mdl", "knife_cord" } },
	{ WEAPON_KNIFE_CANIS, { "Survival Knife", "models/weapons/v_knife_canis.mdl", "knife_canis" } },
	{ WEAPON_KNIFE_URSUS, { "Ursus Knife", "models/weapons/v_knife_ursus.mdl", "knife_ursus" } },
	{ WEAPON_KNIFE_GYPSY_JACKKNIFE, { "Navaja Knife", "models/weapons/v_knife_gypsy_jackknife.mdl", "knife_gypsy_jackknife" } },
	{ WEAPON_KNIFE_OUTDOOR, { "Nomad Knife", "models/weapons/v_knife_outdoor.mdl", "knife_outdoor" } },
	{ WEAPON_KNIFE_STILETTO, { "Stiletto Knife", "models/weapons/v_knife_stiletto.mdl", "knife_stiletto" } },
	{ WEAPON_KNIFE_WIDOWMAKER, { "Talon Knife", "models/weapons/v_knife_widowmaker.mdl", "knife_widowmaker" } },
	{ WEAPON_KNIFE_SKELETON, { "Skeleton Knife", "models/weapons/v_knife_skeleton.mdl", "knife_skeleton" } },
	{ WEAPON_KNIFE_CSS, {"CSS Knife","models/weapons/v_knife_css.mdl","knife_css"} },
};

const std::pair<std::size_t, std::string> arrItemQuality[] =
{
	{ 0, "Normal" },
	{ 1, "Genuine" },
	{ 2, "Vintage" },
	{ 3, "Unusual" },
	{ 5, "Community" },
	{ 6, "Developer" },
	{ 7, "Self-Made" },
	{ 8, "Customized" },
	{ 9, "Strange" },
	{ 10, "Completed" },
	{ 12, "Tournament" }
};

struct SkinKit_t
{
	int m_nID;
	std::string m_szName;
	std::string m_szImagePath;
	int m_nRarity;
	int m_iWeaponID;

	//bool operator < (const SkinKit_t& other) const { return (m_szName < other.m_szName); }
	bool operator<(const SkinKit_t& other) const {
		if (m_iWeaponID == other.m_iWeaponID) {
			return m_nRarity > other.m_nRarity;
		}
		return m_iWeaponID < other.m_iWeaponID;
	}
};

struct SkinColors_t
{
	Color m_colColor[5];
};

// @todo: add valve vdf/vpk parser and get skins, rarity things, etc with it
class CSkinChanger
{
public:

	enum ESequence
	{
		SEQUENCE_DEFAULT_DRAW = 0,
		SEQUENCE_DEFAULT_IDLE1 = 1,
		SEQUENCE_DEFAULT_IDLE2 = 2,
		SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
		SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
		SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
		SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
		SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
		SEQUENCE_DEFAULT_LOOKAT01 = 12,

		SEQUENCE_BUTTERFLY_DRAW = 0,
		SEQUENCE_BUTTERFLY_DRAW2 = 1,
		SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
		SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

		SEQUENCE_FALCHION_IDLE1 = 1,
		SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
		SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
		SEQUENCE_FALCHION_LOOKAT01 = 12,
		SEQUENCE_FALCHION_LOOKAT02 = 13,

		SEQUENCE_CSS_LOOKAT01 = 14,
		SEQUENCE_CSS_LOOKAT02 = 15,

		SEQUENCE_DAGGERS_IDLE1 = 1,
		SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
		SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
		SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
		SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

		SEQUENCE_BOWIE_IDLE1 = 1,
	};

	int m_last_seq;

	inline int RandomSequence(int low, int high)
	{
		return rand() % (high - low + 1) + low;
	}

	auto GetNewAnimation(const uint32_t model, const int sequence) -> int
	{
		switch (model)
		{
		case fnv::HashConst("models/weapons/v_knife_butterfly.mdl"):
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_DRAW:
				return RandomSequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return RandomSequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
			default:
				return sequence + 1;
			}
		}
		case fnv::HashConst("models/weapons/v_knife_falchion_advanced.mdl"):
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_FALCHION_IDLE1;
			case SEQUENCE_DEFAULT_HEAVY_MISS1:
				return RandomSequence(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return RandomSequence(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return sequence;
			default:
				return sequence - 1;
			}
		}
		case fnv::HashConst("models/weapons/v_knife_css.mdl"):
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_LOOKAT01:
				return RandomSequence(SEQUENCE_CSS_LOOKAT01, SEQUENCE_CSS_LOOKAT02);
			default:
				return sequence;
			}
		}
		case fnv::HashConst("models/weapons/v_knife_push.mdl"):
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_DAGGERS_IDLE1;
			case SEQUENCE_DEFAULT_LIGHT_MISS1:
			case SEQUENCE_DEFAULT_LIGHT_MISS2:
				return RandomSequence(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
			case SEQUENCE_DEFAULT_HEAVY_MISS1:
				return RandomSequence(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
			case SEQUENCE_DEFAULT_HEAVY_HIT1:
			case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
			case SEQUENCE_DEFAULT_LOOKAT01:
				return sequence + 3;
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return sequence;
			default:
				return sequence + 2;
			}
		}
		case fnv::HashConst("models/weapons/v_knife_survival_bowie.mdl"):
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return sequence;
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_BOWIE_IDLE1;
			default:
				return sequence - 1;
			}
		}
		case fnv::HashConst("models/weapons/v_knife_ursus.mdl"):
		case fnv::HashConst("models/weapons/v_knife_cord.mdl"):
		case fnv::HashConst("models/weapons/v_knife_canis.mdl"):
		case fnv::HashConst("models/weapons/v_knife_outdoor.mdl"):
		case fnv::HashConst("models/weapons/v_knife_skeleton.mdl"):
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_DRAW:
				return RandomSequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return RandomSequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
			default:
				return sequence + 1;
			}
		}
		case fnv::HashConst("models/weapons/v_knife_stiletto.mdl"):
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_LOOKAT01:
				return RandomSequence(12, 13);
			default:
				return sequence;
			}
		}
		case fnv::HashConst("models/weapons/v_knife_widowmaker.mdl"):
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_LOOKAT01:
				return RandomSequence(14, 15);
			default:
				return sequence;
			}
		}
		default:
			return sequence;
		}
	}

	const std::vector<AgentObject_t> agentList =
	{
		{"models/player/custom_player/legacy/ctm_sas.mdl", "Default"},
		{"models/player/custom_player/legacy/tm_leet_variantf.mdl", "The Elite Mr. Muhlik | Elite Crew"},
		{"models/player/custom_player/legacy/tm_leet_varianti.mdl", "Prof. Shahmat | Elite Crew"},
		{"models/player/custom_player/legacy/tm_leet_varianth.mdl", "Osiris | Elite Crew"},
		{"models/player/custom_player/legacy/tm_leet_variantg.mdl", "Ground Rebel  | Elite Crew"},
		{"models/player/custom_player/legacy/ctm_fbi_variantb.mdl",	"Special Agent Ava | FBI"},
		{"models/player/custom_player/legacy/ctm_fbi_varianth.mdl", "Michael Syfers  | FBI Sniper"},
		{"models/player/custom_player/legacy/ctm_fbi_variantg.mdl", "Markus Delrow | FBI HRT"},
		{"models/player/custom_player/legacy/ctm_fbi_variantf.mdl", "Operator | FBI SWAT"},
		{"models/player/custom_player/legacy/ctm_st6_variante.mdl", "Seal Team 6 Soldier | NSWC SEAL"},
		{"models/player/custom_player/legacy/ctm_st6_variantm.mdl", "'Two Times' McCoy | USAF TACP"},
		{"models/player/custom_player/legacy/ctm_st6_variantg.mdl", "Buckshot | NSWC SEAL"},
		{"models/player/custom_player/legacy/ctm_st6_variantk.mdl", "3rd Commando Company | KSK"},
		{"models/player/custom_player/legacy/ctm_st6_varianti.mdl", "Lt. Commander Ricksaw | NSWC SEAL"},
		{"models/player/custom_player/legacy/ctm_st6_variantj.mdl", "'Blueberries' Buckshot | NSWC SEAL"},
		{"models/player/custom_player/legacy/ctm_st6_variantl.mdl", "'Two Times' McCoy | TACP Cavalry"},
		{"models/player/custom_player/legacy/ctm_swat_variante.mdl","Cmdr. Mae 'Dead Cold' Jamison | SWAT"},
		{"models/player/custom_player/legacy/ctm_swat_variantf.mdl","1st Lieutenant Farlow | SWAT"},
		{"models/player/custom_player/legacy/ctm_swat_variantg.mdl","John 'Van Healen' Kask | SWAT"} ,
		{"models/player/custom_player/legacy/ctm_swat_varianth.mdl","Bio-Haz Specialist | SWAT"},
		{"models/player/custom_player/legacy/ctm_swat_varianti.mdl","Sergeant Bombson | SWAT"},
		{"models/player/custom_player/legacy/ctm_swat_variantj.mdl","Chem-Haz Specialist | SWAT"},
		{"models/player/custom_player/legacy/tm_balkan_varianti.mdl", "Maximus | Sabre"},
		{"models/player/custom_player/legacy/tm_balkan_variantf.mdl", "Dragomir | Sabre"},
		{"models/player/custom_player/legacy/tm_balkan_varianth.mdl", "'The Doctor' Romanov | Sabre"},
		{"models/player/custom_player/legacy/tm_balkan_variantg.mdl",  "Rezan The Ready | Sabre"},
		{"models/player/custom_player/legacy/tm_balkan_variantj.mdl", "Blackwolf | Sabre"},
		{"models/player/custom_player/legacy/tm_balkan_variantk.mdl",  "Rezan the Redshirt | Sabre"},
		{"models/player/custom_player/legacy/tm_balkan_variantl.mdl", "Dragomir | Sabre Footsoldier"},
		{"models/player/custom_player/legacy/ctm_sas_variantf.mdl","B Squadron Officer | SAS"},
		{"models/player/custom_player/legacy/tm_phoenix_varianth.mdl", "Soldier | Phoenix"},
		{"models/player/custom_player/legacy/tm_phoenix_variantf.mdl", "Enforcer | Phoenix"},
		{"models/player/custom_player/legacy/tm_phoenix_variantg.mdl", "Slingshot | Phoenix"},
		{"models/player/custom_player/legacy/tm_phoenix_varianti.mdl", "Street Soldier | Phoenix"},
		{"models/player/custom_player/legacy/tm_professional_varf.mdl", "Sir Bloody Miami Darryl | The Professionals"},
		{"models/player/custom_player/legacy/tm_professional_varf1.mdl", "Sir Bloody Silent Darryl | The Professionals"},
		{"models/player/custom_player/legacy/tm_professional_varf2.mdl", "Sir Bloody Skullhead Darryl | The Professionals"},
		{"models/player/custom_player/legacy/tm_professional_varf3.mdl", "Sir Bloody Darryl Royale | The Professionals"},
		{"models/player/custom_player/legacy/tm_professional_varf4.mdl", "Sir Bloody Loudmouth Darryl | The Professionals"},
		{"models/player/custom_player/legacy/tm_professional_varg.mdl", "Safecracker Voltzmann | The Professionals"},
		{"models/player/custom_player/legacy/tm_professional_varh.mdl", "Little Kev | The Professionals"},
		{"models/player/custom_player/legacy/tm_professional_vari.mdl", "Number K | The Professionals"},
		{"models/player/custom_player/legacy/tm_professional_varj.mdl", "Getaway Sally | The Professionals"}

	};

	// always get from a csgo thread not imgui
	short iItemDefinitionIndex = 0;
	bool bIsKnife = false;

	bool bShouldFullUpdate = false;
	float flUpdateTime = 0.0f;
	// Get
	void Run(CBaseEntity* pLocal);
	/* knife kill icons replace */
	void Event(IGameEvent* pEvent);
	/* dump stikers and paintkits */
	void Dump();
    void AgentChanger(CBaseEntity* pLocal, EStage stage);
	//void DumpStickers(ItemSchema* item_schema);

	int GetMenuFromId(short ID);
	int GetGloveIdFromMenu(int m_iWeaponIndex);
	int GetIdFromMenu(short menu);
	int GetKnifeDefinitionIndex(int m_iWeaponIndex);

	ItemSchema_t* m_pItemSchematic;

	/*containers*/
	std::vector<SkinKit_t> SkinKits;
	std::vector<SkinKit_t> GloveKits;
	std::vector<SkinKit_t> StickerKits;
	std::vector<SkinColors_t> SkinColors;
private:

	/* ur values and main functionality */
	bool IsKnife(int i)
	{
		return (i >= WEAPON_KNIFE_BAYONET && i < GLOVE_STUDDED_BROKENFANG) || i == WEAPON_KNIFE_T || i == WEAPON_KNIFE;
	}

	bool ApplyKnifeModel(CBaseCombatWeapon* pWeapon, const char* szModel);
	bool ApplyKnifeSkin(CBaseCombatWeapon* pWeapon, const char* szModel, short iItemDefIndex);

	void ApplyStickers(CBaseCombatWeapon* pWeapon);
	/* ur values and main functionality */
};
inline CSkinChanger skinChanger;