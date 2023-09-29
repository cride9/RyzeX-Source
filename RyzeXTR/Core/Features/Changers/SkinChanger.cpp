#include "SkinChanger.h"

using namespace cfg::skin;

class CCStrike15ItemSchema;
class CCStrike15ItemSystem;

template <typename Key, typename Value>
struct Node_t
{
	int previous_id;		//0x0000
	int next_id;			//0x0004
	void* _unknown_ptr;		//0x0008
	int _unknown;			//0x000C
	Key key;				//0x0010
	Value value;			//0x0014
};

template <typename Key, typename Value>
struct Head_t
{
	Node_t<Key, Value>* memory;		//0x0000
	int allocation_count;			//0x0004
	int grow_size;					//0x0008
	int start_element;				//0x000C
	int next_available;				//0x0010
	int _unknown;					//0x0014
	int last_element;				//0x0018
}; //Size=0x001C

// could use CUtlString but this is just easier and CUtlString isn't needed anywhere else
struct String_t
{
	char* buffer;	//0x0000
	int capacity;	//0x0004
	int grow_size;	//0x0008
	int length;		//0x000C
}; //Size=0x0010

//void CSkinChanger::ApplyStickers(CBaseCombatWeapon* pWeapon)
//{
//	static uint16_t s_econ_item_interface_wrapper_offset = std::uint16_t(0);
//
//	if (!s_econ_item_interface_wrapper_offset)
//		s_econ_item_interface_wrapper_offset = CNetvarManager::Get().mapProps[FNV1A::HashConst("CBaseAttributableItem->m_Item")].uOffset + 0xC; // might crash
//
//	const unsigned int econ_item_interface_wrapper = std::uintptr_t(pWeapon) + s_econ_item_interface_wrapper_offset;
//
//	if (!DTR::GetStickerAttributeBySlotIndexInt.IsHooked())
//		DTR::GetStickerAttributeBySlotIndexInt.Create(MEM::GetVFunc(reinterpret_cast<void*>(econ_item_interface_wrapper), 5), &H::hkGetStickerAttributeBySlotIndexInt);
//}

CreateClientClassFn GetWearableCreateFn()
{
	auto client_class = i::ClientDll->GetAllClasses();
	for (client_class = i::ClientDll->GetAllClasses();
		client_class; client_class = client_class->pNext)
	{
		if (client_class->nClassID == EClassIndex::CEconWearable)
			return client_class->pCreateFn;
	}
}

CBaseCombatWeapon* MakeGlove(int entry, int serial)
{
	static CreateClientClassFn WearableCreateFn = GetWearableCreateFn();
	WearableCreateFn(entry, serial);

	const auto glove = static_cast<CBaseCombatWeapon*>(i::EntityList->GetClientEntity(entry));
	assert(glove);
	{
		static auto set_abs_origin_addr = MEM::FindPattern(CLIENT_DLL, "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8");
		const auto set_abs_origin_fn = reinterpret_cast<void(__thiscall*)(void*, const std::array<float, 3>&)>(set_abs_origin_addr);
		static constexpr std::array<float, 3> new_pos = { 10000.f, 10000.f, 10000.f };
		set_abs_origin_fn(glove, new_pos);
	}
	return glove;
}

bool ApplyGlove(CBaseCombatWeapon* glove, short item_definition_index, int paint_kit, int model_index, int entity_quality, float fallback_wear) {
	auto oldIndex = glove->GetItemDefinitionIndex();

	if (oldIndex != item_definition_index)
		glove->GetItemDefinitionIndex() = item_definition_index;

	glove->GetFallbackPaintKit() = paint_kit;
	glove->GetModelIndex() = model_index;
	glove->SetModelIndex(model_index);
	glove->GetEntityQuality() = entity_quality;
	glove->GetFallbackWear() = fallback_wear;

	return true;
}

void CSkinChanger::Dump()
{
	const auto V_UCS2ToUTF8 = static_cast<int(*)(const wchar_t* ucs2, char* utf8, int len)>(reinterpret_cast<void*>(MEM::GetExportAddress(MEM::GetModuleBaseHandle(VSTDLIB_DLL), XorStr("V_UCS2ToUTF8"))));
	std::ifstream items = std::ifstream(XorStr("csgo/scripts/items/items_game_cdn.txt"));
	std::string gameItems = std::string(std::istreambuf_iterator <char> { items }, std::istreambuf_iterator <char> { });

	if (!items.is_open())
		return;

	items.close();

	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	uintptr_t sig_address = reinterpret_cast<uintptr_t>(MEM::FindPattern(CLIENT_DLL, "E8 ?? ?? ?? ?? FF 76 0C 8D 48 04 E8"));

	// Skip the opcode, read rel32 address
	int item_system_offset = *reinterpret_cast<int32_t*>(sig_address + 1);

	// Add the offset to the end of the instruction
	auto item_system_fn = reinterpret_cast<CCStrike15ItemSystem * (*)()>(sig_address + 5 + item_system_offset);

	// Skip VTable, first member variable of ItemSystem is ItemSchema
	m_pItemSchematic = reinterpret_cast<ItemSchema_t*>(uintptr_t(item_system_fn()) + sizeof(void*));

	std::vector<std::pair<short, short>> kitsWeapons;
	kitsWeapons.reserve(m_pItemSchematic->alternateIcons.numElements);

	for (const auto& node : m_pItemSchematic->alternateIcons) // https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/shared/econ/econ_item_schema.cpp#L325-L329
	{
		if (const auto encoded = node.key; (encoded & 3) == 0)
			kitsWeapons.emplace_back(static_cast<short>((encoded & 0xFFFF) >> 2), static_cast<short>(encoded >> 16)); // , node.value.simpleName.data()
	}

	std::sort(kitsWeapons.begin(), kitsWeapons.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

	std::unordered_map<short, std::wstring> weaponNames;
	for (const auto& kitWeapon : kitsWeapons)
	{
		if (weaponNames.contains(kitWeapon.second))
			continue;

		if (const auto itemDef = m_pItemSchematic->getItemDefinitionInterface(kitWeapon.second)) {
			weaponNames.emplace(kitWeapon.second, i::Localize->Find(itemDef->getItemBaseName()));
		}
	}

	SkinKits.reserve(m_pItemSchematic->m_pPaintKits.lastAlloc);
	GloveKits.reserve(m_pItemSchematic->m_pPaintKits.lastAlloc);

	for (int i = 0; i <= m_pItemSchematic->m_pPaintKits.lastAlloc; i++)
	{
		const auto paintKit = m_pItemSchematic->m_pPaintKits.memory[i].value;

		if (paintKit->m_nID == 0 || paintKit->m_nID == 9001) // ignore workshop_default
			continue;

		if (paintKit->m_nID >= 10000)
		{
			const std::string_view gloveName{ paintKit->m_szName.data() };
			std::wstring name;

			if (gloveName._Starts_with("bloodhound"))
				name = i::Localize->Find("CSGO_Wearable_t_studdedgloves");
			else if (gloveName._Starts_with("motorcycle"))
				name = i::Localize->Find("CSGO_Wearable_v_motorcycle_glove");
			else if (gloveName._Starts_with("slick"))
				name = i::Localize->Find("CSGO_Wearable_v_slick_glove");
			else if (gloveName._Starts_with("sporty"))
				name = i::Localize->Find("CSGO_Wearable_v_sporty_glove");
			else if (gloveName._Starts_with("specialist"))
				name = i::Localize->Find("CSGO_Wearable_v_specialist_glove");
			else if (gloveName._Starts_with("operation10"))
				name = i::Localize->Find("CSGO_Wearable_t_studded_brokenfang_gloves");
			else if (gloveName._Starts_with("handwrap"))
				name = i::Localize->Find("CSGO_Wearable_v_leather_handwrap");
			else
				assert(false);

			name += L" | ";
			name += i::Localize->Find(paintKit->m_szDescriptionTag.data() + 1);

			char nameStr[256];
			V_UCS2ToUTF8(name.c_str(), nameStr, sizeof(nameStr));

			GloveKits.push_back({ paintKit->m_nID, nameStr, paintKit->m_szName.data(), paintKit->m_nRarity,0 });
		}
		else
		{
			std::unordered_set<short> weapons;

			for (auto it = std::lower_bound(kitsWeapons.begin(), kitsWeapons.end(), paintKit->m_nID, [](const auto& p, auto id) { return p.first < id; }); it != kitsWeapons.end() && it->first == paintKit->m_nID; ++it)
			{
				const auto itemDef = m_pItemSchematic->getItemDefinitionInterface(it->second);
				if (!itemDef)
					continue;

				std::wstring name = i::Localize->Find(itemDef->getItemBaseName());
				name += L" | ";
				name += i::Localize->Find(paintKit->m_szDescriptionTag.data() + 1);
				//std::wstring szWeaponName = weaponNames[it->second]; // in case we want to use weapon names separately in the future...

				char nameStr[256];
				V_UCS2ToUTF8(name.c_str(), nameStr, sizeof(nameStr));
				SkinKits.push_back({ paintKit->m_nID, nameStr, paintKit->m_szName.data() ,std::clamp(itemDef->getRarity() + paintKit->m_nRarity - 1, 0, (paintKit->m_nRarity == 7) ? 7 : 6),(int)it->second });
			}
		}
	}

	std::sort(SkinKits.begin(), SkinKits.end());
	SkinKits.shrink_to_fit();
	std::sort(GloveKits.begin(), GloveKits.end());
	GloveKits.shrink_to_fit();

	//Handle stickers
	StickerKits.reserve(m_pItemSchematic->stickerKits.lastAlloc);
	for (int i = 0; i <= m_pItemSchematic->stickerKits.lastAlloc; i++)
	{
		const auto stickerKit = m_pItemSchematic->stickerKits.memory[i].value;
		if (std::string_view name{ stickerKit->name.data() }; name._Starts_with("spray") || name._Starts_with("patch"))
			continue;
		std::wstring name = i::Localize->Find(stickerKit->id != 242 ? stickerKit->itemName.data() + 1 : "StickerKit_dhw2014_teamdignitas_gold");

		char nameStr[256];
		V_UCS2ToUTF8(name.c_str(), nameStr, sizeof(nameStr));

		StickerKits.push_back({ stickerKit->id, std::move(nameStr),stickerKit->inventoryImage.data(), stickerKit->rarity,0 });
	}

	StickerKits.insert(StickerKits.begin(), { 0, "None" });
	std::sort(StickerKits.begin() + 1, StickerKits.end());
	StickerKits.shrink_to_fit();

	for (size_t m{ 0 }; m < m_pItemSchematic->m_pPaintKits.lastAlloc; m++)
	{
		PaintKit_t* m_pPaintKit = m_pItemSchematic->m_pPaintKits.memory[m].value;

		SkinColors_t inf = SkinColors_t();
		inf.m_colColor[0] = m_pPaintKit->m_rgbaColor[0];
		inf.m_colColor[1] = m_pPaintKit->m_rgbaColor[1];
		inf.m_colColor[2] = m_pPaintKit->m_rgbaColor[2];
		inf.m_colColor[3] = m_pPaintKit->m_rgbaColor[3];
		SkinColors.push_back(inf);
	}
}

void CSkinChanger::AgentChanger(CBaseEntity* pLocal, EStage stage)
{
	if (!bEnableSkinChanger)
		return;

	if (!iSkinId[36])
		return;

	//pLocal->SetModelIndex(i::ModelInfo->GetModelIndex(agentList.at(iSkinId[36]).szModelName.c_str()));

	//return;
	static int originalIdx = 0;

	if (!pLocal)
	{
		originalIdx = 0;
		return;
	}

	if (const auto model = agentList.at(iSkinId[36]).szModelName.c_str())
	{
		if (stage == FRAME_RENDER_START)
			originalIdx = i::ModelInfo->GetModelIndex(pLocal->GetModel()->szName);

		const auto idx = stage == FRAME_RENDER_END && originalIdx ? originalIdx : i::ModelInfo->GetModelIndex(model);
		pLocal->SetModelIndex(idx);
	}
}

void HandleGloves()
{
	if (!iGloveModel)
		return;

	CBaseEntity* pLocal = CBaseEntity::GetLocalPlayer();

	if (pLocal == nullptr)
		return;

	CBaseHandle* wearables = pLocal->GetWearablesHandle();
	if (!wearables)
		return;

	static uintptr_t glove_handle = uintptr_t(0);

	CBaseCombatWeapon* glove = reinterpret_cast<CBaseCombatWeapon*>(i::EntityList->GetClientEntityFromHandle(wearables[0]));

	if (!glove)
	{
		auto our_glove = reinterpret_cast<CBaseCombatWeapon*>(i::EntityList->GetClientEntityFromHandle(glove_handle));
		if (our_glove)
		{
			wearables[0] = glove_handle;
			glove = our_glove;
		}
	}

	if (!pLocal->IsAlive())
	{
		if (glove)
		{
			glove->SetDestroyedOnRecreateEntities();
			glove->ReleaseNetworkable();
		}
		return;
	}

	if (!glove)
	{
		const int highestEntityIndex = i::EntityList->GetHighestEntityIndex();
		const int entry = highestEntityIndex + 1;
		const int serial = rand() % 0x1000;
		glove = MakeGlove(entry, serial);   // He he
		wearables[0] = entry | serial << 16;
		glove_handle = wearables[0]; // Let's store it in case we somehow lose it.
	}

	if (glove)
	{
		ApplyGlove(glove, skinChanger.GetGloveIdFromMenu(iGloveModel), iSkinId[35], i::ModelInfo->GetModelIndex(mapGloveList.at(skinChanger.GetGloveIdFromMenu(iGloveModel))), 0, flSkinWear[35]);
		
		uint64_t uMask = UINT32_MAX;
		uint32_t uLowId = uMask & skinChanger.GetGloveIdFromMenu(iGloveModel);
		uint32_t uHighId = skinChanger.GetGloveIdFromMenu(iGloveModel) >> 32;

		glove->GetItemIDLow() = uLowId;
		glove->GetItemIDHigh() = uHighId;
		glove->GetAccountID() = pLocal->GetPlayerInfo().nXuidLow;

		if (EconItemDefinition* def = skinChanger.m_pItemSchematic->getItemDefinitionInterface(skinChanger.GetGloveIdFromMenu(iGloveModel)); def)
			if (CBaseEntity* pViewModel = static_cast<CBaseEntity*>(i::EntityList->GetClientEntityFromHandle(pLocal->GetViewModel())); pViewModel)
				if (const char* szViewmodel = def->getPlayerDisplayModel(); szViewmodel)
					if (CBaseHandle pWorldModelHandle = glove->GetRefEHandle(); pWorldModelHandle)
						if (CBaseCombatWeapon* pWorldModel = (CBaseCombatWeapon*)(i::EntityList->GetClientEntityFromHandle(pWorldModelHandle)); pWorldModel)
							if (const char* szWorldModel = def->getWorldDisplayModel(); szWorldModel)
								glove->SetModelIndex(i::ModelInfo->GetModelIndex(szWorldModel));

		static auto uEquipSig = MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 EC 10 53 8B 5D 08 57 8B F9"));
		static auto uInitializeAttributesSig = MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 83 EC 0C 53 56 8B F1 8B 86"));

		((int(__thiscall*)(void*, void*))(uEquipSig))(glove, pLocal);

		pLocal->GetBodyID() = 1;

		((int(__thiscall*)(void*))(uInitializeAttributesSig))(glove);

		i::LeafSystem->CreateRenderableHandle(glove, true, 0, RENDERABLE_MODEL_UNKNOWN_TYPE);

		glove->GetClientNetworkable()->PreDataUpdate(DATA_UPDATE_CREATED);
	}
}

bool CSkinChanger::ApplyKnifeModel(CBaseCombatWeapon* pWeapon, const char* szModel)
{
	if (g::pLocal == nullptr)
		return false;

	CBaseViewModel* pViewmodel = (CBaseViewModel*)i::EntityList->GetClientEntityFromHandle(g::pLocal->GetViewModel());
	if (!pViewmodel)
		return false;

	CBaseHandle pWeaponHandle = pViewmodel->GetWeaponHandle();
	if (!pWeaponHandle)
		return false;

	CBaseCombatWeapon* pViewmodelWeapon = (CBaseCombatWeapon*)(i::EntityList->GetClientEntityFromHandle(pWeaponHandle));
	if (pViewmodelWeapon != pWeapon)
		return false;

	pViewmodel->GetModelIndex() = i::ModelInfo->GetModelIndex(szModel);

	return true;
}

bool CSkinChanger::ApplyKnifeSkin(CBaseCombatWeapon* pWeapon, const char* szModel, short iItemDefIndex)
{
	if (g::pLocal == nullptr)
		return false;

	pWeapon->GetItemDefinitionIndex() = iItemDefIndex;
	pWeapon->GetEntityQuality() = 3;
	pWeapon->GetModelIndex() = i::ModelInfo->GetModelIndex(szModel);

	EconItemDefinition* def = m_pItemSchematic->getItemDefinitionInterface(iItemDefIndex);
	if (!def)
		return false;

	CBaseEntity* pViewModel = static_cast<CBaseEntity*>(i::EntityList->GetClientEntityFromHandle(g::pLocal->GetViewModel()));
	if (!pViewModel)
		return false;

	const char* szViewmodel = def->getPlayerDisplayModel();
	if (!szViewmodel)
		return false;

	CBaseHandle pWorldModelHandle = pWeapon->GetWorldModelHandle();
	if (!pWorldModelHandle)
		return false;

	CBaseCombatWeapon* pWorldModel = (CBaseCombatWeapon*)(i::EntityList->GetClientEntityFromHandle(pWorldModelHandle));
	if (!pWorldModel)
		return false;

	const char* szWorldModel = def->getWorldDisplayModel();
	if (!szWorldModel)
		return false;

	pWorldModel->GetModelIndex() = i::ModelInfo->GetModelIndex(szWorldModel);

	return true;
}

void ForceItemUpdate(CBaseCombatWeapon* m_pWeapon)
{
	if (!m_pWeapon || !m_pWeapon->IsWeapon())
		return;

	if (i::ClientState->iDeltaTick == -1)
		return;

	m_pWeapon->CustomMaterialInitialized( ) = m_pWeapon->GetFallbackPaintKit( ) <= 0;
	m_pWeapon->CustomMaterialInitialized() = false;

	//m_pWeapon->GetCustomMaterials().RemoveAll();
	//m_pWeapon->GetCustomMaterials2().RemoveAll();

	size_t count = m_pWeapon->GetVisualsDataProcessors().Count();
	for (size_t i{ }; i < count; ++i)
	{
		auto& elem = m_pWeapon->GetVisualsDataProcessors()[i];
		if (elem)
		{
			elem->Release();
			elem = nullptr;
		}
	}

	m_pWeapon->GetVisualsDataProcessors().RemoveAll();

	m_pWeapon->PostDataUpdate(DATA_UPDATE_CREATED);
	m_pWeapon->OnDataChanged(DATA_UPDATE_CREATED);

	i::ClientState->iDeltaTick = -1;

	util::ForceFullUpdate();
}

void CSkinChanger::Run(CBaseEntity* pLocal)
{
	if (!bEnableSkinChanger)
		return;

	if (pLocal == nullptr)
		return;

	HandleGloves();

	if (!pLocal->IsAlive())
		return;

	PlayerInfo_t pInfo;
	if (!i::EngineClient->GetPlayerInfo(i::EngineClient->GetLocalPlayer(), &pInfo))
		return;

	auto pWeapon = pLocal->GetWeapon();

	if (!pWeapon)
		return;

	std::vector< CBaseCombatWeapon* > pWeapons{ };

	//--------------------------------------------definitions-------------------------------------------- 

	int index_knifeCT = i::ModelInfo->GetModelIndex(mapItemList.at(WEAPON_KNIFE).szModel);
	int index_knifeT = i::ModelInfo->GetModelIndex(mapItemList.at(WEAPON_KNIFE_T).szModel);

	int XUID = pInfo.nXuidLow;

	//--------------------------------------------definitions-------------------------------------------- 

	//--------------------------------------------glove-changer--------------------------------------------

	//--------------------------------------------weapon-changer-------------------------------------------- 

	if (CBaseHandle* hWeapons = reinterpret_cast<CBaseHandle*>(pLocal->GetWeaponsHandle().data()); hWeapons != nullptr)
	{
		// -1 to prevent double active weapon
		for (int nIndex = MAX_WEAPONS - 1; hWeapons[nIndex]; nIndex--)
		{
			// get current weapon
			CBaseCombatWeapon* pCurrentWeapon = reinterpret_cast<CBaseCombatWeapon*>(i::EntityList->GetClientEntityFromHandle(hWeapons[nIndex]));

			if (pCurrentWeapon == nullptr)
				continue;

			short& nDefinitionIndex = pCurrentWeapon->GetItemDefinitionIndex();

			CCSWeaponInfo* pWeaponData = i::WeaponSystem->GetWpnData(nDefinitionIndex);

			if (pWeaponData == nullptr || (pWeaponData->nWeaponType == WEAPONTYPE_GRENADE) || pWeaponData->nWeaponType == WEAPONTYPE_C4 || nDefinitionIndex == WEAPON_TASER)
				continue;

			int menuId = GetMenuFromId(nDefinitionIndex);

			for (size_t m{ 0 }; m < m_pItemSchematic->m_pPaintKits.lastAlloc; m++)
			{
				PaintKit_t* m_pPaintKit = m_pItemSchematic->m_pPaintKits.memory[m].value;

				if (m_pPaintKit->m_nID == iSkinId[menuId])
				{
					if (bModifySkinColors[menuId])
					{
						m_pPaintKit->m_rgbaColor[0] = colSkins1[menuId];
						m_pPaintKit->m_rgbaColor[1] = colSkins2[menuId];
						m_pPaintKit->m_rgbaColor[2] = colSkins3[menuId];
						m_pPaintKit->m_rgbaColor[3] = colSkins4[menuId];
					}
					else
					{
						m_pPaintKit->m_rgbaColor[0] = SkinColors[m].m_colColor[0];
						m_pPaintKit->m_rgbaColor[1] = SkinColors[m].m_colColor[1];
						m_pPaintKit->m_rgbaColor[2] = SkinColors[m].m_colColor[2];
						m_pPaintKit->m_rgbaColor[3] = SkinColors[m].m_colColor[3];
					}
				}
			}

			// Change knife model.
			if (pWeapon->GetClientClass()->nClassID == EClassIndex::CKnife && iKnifeModel > 0)
				ApplyKnifeModel(pCurrentWeapon, mapItemList.at(GetKnifeDefinitionIndex(iKnifeModel)).szModel);

			// Apply knife skin.
			if (pCurrentWeapon->GetClientClass()->nClassID == EClassIndex::CKnife && iKnifeModel > 0)
				ApplyKnifeSkin(pCurrentWeapon, mapItemList.at(GetKnifeDefinitionIndex(iKnifeModel)).szModel, GetKnifeDefinitionIndex(iKnifeModel));

			if (!szSkinNametag[menuId].empty())
				strcpy(pCurrentWeapon->GetCustomName(), szSkinNametag[menuId].c_str());

			if (iSkinStattrak[menuId])
			{
				pCurrentWeapon->GetFallbackStatTrak() = iSkinStattrak[menuId];
				pCurrentWeapon->GetEntityQuality() = 9;
			}
			else
			{
				if (pCurrentWeapon->GetClientClass()->nClassID == EClassIndex::CKnife)
					pCurrentWeapon->GetEntityQuality() = 3;
				else
					pCurrentWeapon->GetEntityQuality() = 0;
			}

			pCurrentWeapon->GetFallbackPaintKit() = iSkinId[menuId];
			pCurrentWeapon->GetFallbackWear() = flSkinWear[menuId] / 100.f;

			pCurrentWeapon->GetOwnerXuidHigh() = 0;
			pCurrentWeapon->GetOwnerXuidLow() = 0;
			pCurrentWeapon->GetAccountID() = XUID;
			pCurrentWeapon->GetFallbackSeed() = iSeed[menuId];
			pCurrentWeapon->GetItemIDHigh() = -1;
			pCurrentWeapon->GetEntityQuality() = arrItemQuality[iQuality[menuId]].first;

			//ApplyStickers(pCurrentWeapon);

			// weapon that we own
			pWeapons.push_back(pCurrentWeapon);
		}
	}

	//--------------------------------------------weapon-changer-------------------------------------------- 

	// we only want to force update every second rather than spam constant which can cause crash
	if (bShouldFullUpdate && i::GlobalVars->flCurrentTime >= flUpdateTime)
	{
		g::bUpdatingSkins = true;
		//for (auto& w : pWeapons)
		//	ForceItemUpdate(w);

		int menuId = GetMenuFromId(g::pLocal->GetWeapon()->GetItemDefinitionIndex());

		for (size_t m{ 0 }; m < m_pItemSchematic->m_pPaintKits.lastAlloc; m++)
		{
			PaintKit_t* m_pPaintKit = m_pItemSchematic->m_pPaintKits.memory[m].value;

			if (m_pPaintKit->m_nID == iSkinId[menuId])
			{
				if (bModifySkinColors[menuId])
				{
					m_pPaintKit->m_rgbaColor[0] = colSkins1[menuId];
					m_pPaintKit->m_rgbaColor[1] = colSkins2[menuId];
					m_pPaintKit->m_rgbaColor[2] = colSkins3[menuId];
					m_pPaintKit->m_rgbaColor[3] = colSkins4[menuId];
				}
				else
				{
					m_pPaintKit->m_rgbaColor[0] = SkinColors[m].m_colColor[0];
					m_pPaintKit->m_rgbaColor[1] = SkinColors[m].m_colColor[1];
					m_pPaintKit->m_rgbaColor[2] = SkinColors[m].m_colColor[2];
					m_pPaintKit->m_rgbaColor[3] = SkinColors[m].m_colColor[3];
				}
			}
		}

		//i::ClientDllState->m_iDeltaTick = -1;
		if (g::pLocal != nullptr && g::pLocal->GetWeapon() != nullptr)
			ForceItemUpdate(g::pLocal->GetWeapon());

		bShouldFullUpdate = false;
		flUpdateTime = i::GlobalVars->flCurrentTime + 1.f;

		g::bUpdatingSkins = false;
	}

	// it will help you here my sweetest candy <3
	// https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Skin_Changer
}

void CSkinChanger::Event(IGameEvent* pEvent)
{
	if (!bEnableSkinChanger)
		return;

	if (pEvent == nullptr || !i::EngineClient->IsInGame())
		return;

	if (g::pLocal == nullptr || !g::pLocal->IsAlive())
		return;

	auto pWeapon = g::pLocal->GetWeapon();

	if (!pWeapon)
		return;

	auto defIndex = pWeapon->GetItemDefinitionIndex();

	auto menuId = GetMenuFromId(defIndex);

	const std::string_view uNameHash = pEvent->GetName();

	/* update stattrak */
	if (uNameHash.find(cachedEvents::playerDeath) != std::string_view::npos)
	{
		CBaseEntity* pAttacker = reinterpret_cast<CBaseEntity*>(i::EntityList->GetClientEntity(i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("attacker")))));

		if (pAttacker == g::pLocal)
		{
			CBaseEntity* pEntity = reinterpret_cast<CBaseEntity*>(i::EntityList->GetClientEntity(i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("userid")))));

			if (pEntity != nullptr && pEntity != g::pLocal)
			{
				if (!IsKnife(defIndex))
					return;

				CCSWeaponInfo* pWeaponData = i::WeaponSystem->GetWpnData(g::pLocal->GetWeapon()->GetItemDefinitionIndex());
				if (!pWeaponData)
					return;

				if (pEvent->GetString(XorStr("weapon"), XorStr("knife")) && pWeaponData->nWeaponType == WEAPONTYPE_KNIFE)
				{
					switch (iKnifeModel)
					{
					case 0:
						pEvent->SetString("weapon", "knife");
						break;
					case 1:
						pEvent->SetString("weapon", "bayonet");
						break;
					case 2:
						pEvent->SetString("weapon", "knife_m9_bayonet");
						break;
					case 3:
						pEvent->SetString("weapon", "knife_karambit");
						break;
					case 4:
						pEvent->SetString("weapon", "knife_survival_bowie");
						break;
					case 5:
						pEvent->SetString("weapon", "knife_butterfly");
						break;
					case 6:
						pEvent->SetString("weapon", "knife_falchion");
						break;
					case 7:
						pEvent->SetString("weapon", "knife_flip");
						break;
					case 8:
						pEvent->SetString("weapon", "knife_gut");
						break;
					case 9:
						pEvent->SetString("weapon", "knife_tactical");
						break;
					case 10:
						pEvent->SetString("weapon", "knife_push");
						break;
					case 11:
						pEvent->SetString("weapon", "knife_gypsy_jackknife");
						break;
					case 12:
						pEvent->SetString("weapon", "knife_stiletto");
						break;
					case 13:
						pEvent->SetString("weapon", "knife_widowmaker");
						break;
					case 14:
						pEvent->SetString("weapon", "knife_ursus");
						break;
					case 15:
						pEvent->SetString("weapon", "knife_cord");
						break;
					case 16:
						pEvent->SetString("weapon", "knife_canis");
						break;
					case 17:
						pEvent->SetString("weapon", "knife_outdoor");
						break;
					case 18:
						pEvent->SetString("weapon", "knife_skeleton");
						break;
					case 19:
						pEvent->SetString("weapon", "knife_css");
						break;
					}
				}

				if (iSkinStattrak[menuId])
				{
					iSkinStattrak[menuId]++;
					ForceItemUpdate(pWeapon);
				}
			}
		}
	}

	/* reset update timer to 0 on round start */
	if (uNameHash.find(cachedEvents::roundStart) != std::string_view::npos)
	{
		flUpdateTime = 0.0f;
	}
}

int CSkinChanger::GetMenuFromId(short ID) {
	switch (ID)
	{
	case WEAPON_KNIFE_BAYONET:
	case WEAPON_KNIFE_M9_BAYONET:
	case WEAPON_KNIFE_KARAMBIT:
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
	case WEAPON_KNIFE_BUTTERFLY:
	case WEAPON_KNIFE_FALCHION:
	case WEAPON_KNIFE_FLIP:
	case WEAPON_KNIFE_GUT:
	case WEAPON_KNIFE_TACTICAL:
	case WEAPON_KNIFE_PUSH:
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:
	case WEAPON_KNIFE_STILETTO:
	case WEAPON_KNIFE_WIDOWMAKER:
	case WEAPON_KNIFE_URSUS:
	case WEAPON_KNIFE_CORD:
	case WEAPON_KNIFE_CANIS:
	case WEAPON_KNIFE_OUTDOOR:
	case WEAPON_KNIFE_SKELETON:
	case WEAPON_KNIFE_CSS:
	case WEAPON_KNIFE_T:
	case WEAPON_KNIFE:
		return 0;
	case WEAPON_DEAGLE:
		return 1;
	case WEAPON_ELITE:
		return 2;
	case WEAPON_FIVESEVEN:
		return 3;
	case WEAPON_GLOCK:
		return 4;
	case WEAPON_TEC9:
		return 5;
	case WEAPON_USP_SILENCER:
		return 6;
	case WEAPON_CZ75A:
		return 7;
	case WEAPON_REVOLVER:
		return 8;
	case WEAPON_HKP2000:
		return 9;
	case WEAPON_P250:
		return 10;
	case WEAPON_AK47:
		return 11;
	case WEAPON_AUG:
		return 12;
	case WEAPON_M4A1:
		return 13;
	case WEAPON_FAMAS:
		return 14;
	case WEAPON_GALILAR:
		return 15;
	case WEAPON_SG556:
		return 16;
	case WEAPON_M4A1_SILENCER:
		return 17;
	case WEAPON_P90:
		return 18;
	case WEAPON_MP5SD:
		return 19;
	case WEAPON_UMP45:
		return 20;
	case WEAPON_MAC10:
		return 21;
	case WEAPON_BIZON:
		return 22;
	case WEAPON_MP7:
		return 23;
	case WEAPON_MP9:
		return 24;
	case WEAPON_AWP:
		return 25;
	case WEAPON_SCAR20:
		return 26;
	case WEAPON_SSG08:
		return 27;
	case WEAPON_G3SG1:
		return 28;
	case WEAPON_M249:
		return 29;
	case WEAPON_NEGEV:
		return 30;
	case WEAPON_XM1014:
		return 31;
	case WEAPON_MAG7:
		return 32;
	case WEAPON_SAWEDOFF:
		return 33;
	case WEAPON_NOVA:
		return 34;
	}
}
int CSkinChanger::GetGloveIdFromMenu(int m_iWeaponIndex)
{
	switch (m_iWeaponIndex)
	{
	case 0:
		break;
	case 1:
		return GLOVE_STUDDED_BLOODHOUND;
	case 2:
		return GLOVE_SPORTY;
	case 3:
		return GLOVE_SLICK;
	case 4:
		return GLOVE_LEATHER_HANDWRAPS;
	case 5:
		return GLOVE_MOTORCYCLE;
	case 6:
		return GLOVE_SPECIALIST;
	case 7:
		return GLOVE_STUDDED_HYDRA;
	case 8:
		return GLOVE_STUDDED_BROKENFANG;
	}
}
int CSkinChanger::GetIdFromMenu(short menu)
{
	switch (menu)
	{
	case 1:
		return WEAPON_DEAGLE;
	case 2:
		return WEAPON_ELITE;
	case 3:
		return WEAPON_FIVESEVEN;
	case 4:
		return WEAPON_GLOCK;
	case 5:
		return WEAPON_TEC9;
	case 6:
		return WEAPON_USP_SILENCER;
	case 7:
		return WEAPON_CZ75A;
	case 8:
		return WEAPON_REVOLVER;
	case 9:
		return WEAPON_HKP2000;
	case 10:
		return WEAPON_P250;
	case 11:
		return WEAPON_AK47;
	case 12:
		return WEAPON_AUG;
	case 13:
		return WEAPON_M4A1;
	case 14:
		return WEAPON_FAMAS;
	case 15:
		return WEAPON_GALILAR;
	case 16:
		return WEAPON_SG556;
	case 17:
		return WEAPON_M4A1_SILENCER;
	case 18:
		return WEAPON_P90;
	case 19:
		return WEAPON_MP5SD;
	case 20:
		return WEAPON_UMP45;
	case 21:
		return WEAPON_MAC10;
	case 22:
		return WEAPON_BIZON;
	case 23:
		return WEAPON_MP7;
	case 24:
		return WEAPON_MP9;
	case 25:
		return WEAPON_AWP;
	case 26:
		return WEAPON_SCAR20;
	case 27:
		return WEAPON_SSG08;
	case 28:
		return WEAPON_G3SG1;
	case 29:
		return WEAPON_M249;
	case 30:
		return WEAPON_NEGEV;
	case 31:
		return WEAPON_XM1014;
	case 32:
		return WEAPON_MAG7;
	case 33:
		return WEAPON_SAWEDOFF;
	case 34:
		return WEAPON_NOVA;
	default:
		return -1; // Return -1 or any other value to indicate an invalid menu value.
	}
}
int CSkinChanger::GetKnifeDefinitionIndex(int m_iWeaponIndex)
{
	switch (m_iWeaponIndex)
	{
	case 1:
		return WEAPON_KNIFE_BAYONET;
	case 2:
		return WEAPON_KNIFE_M9_BAYONET;
	case 3:
		return WEAPON_KNIFE_KARAMBIT;
	case 4:
		return WEAPON_KNIFE_SURVIVAL_BOWIE;
	case 5:
		return WEAPON_KNIFE_BUTTERFLY;
	case 6:
		return WEAPON_KNIFE_FALCHION;
	case 7:
		return WEAPON_KNIFE_FLIP;
	case 8:
		return WEAPON_KNIFE_GUT;
	case 9:
		return WEAPON_KNIFE_TACTICAL;
	case 10:
		return WEAPON_KNIFE_PUSH;
	case 11:
		return WEAPON_KNIFE_GYPSY_JACKKNIFE;
	case 12:
		return WEAPON_KNIFE_STILETTO;
	case 13:
		return WEAPON_KNIFE_WIDOWMAKER;
	case 14:
		return WEAPON_KNIFE_URSUS;
	case 15:
		return WEAPON_KNIFE_CORD;
	case 16:
		return WEAPON_KNIFE_CANIS;
	case 17:
		return WEAPON_KNIFE_OUTDOOR;
	case 18:
		return WEAPON_KNIFE_SKELETON;
	case 19:
		return WEAPON_KNIFE_CSS;
	}
}