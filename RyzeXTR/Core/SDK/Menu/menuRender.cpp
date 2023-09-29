#include "gui.h"
#include "../../Features/Rage/antiaim.h"
#include "../../Features/Changers/SkinChanger.h"
#include "../../Lua/Lua.h"
#include <iostream>
#include <string>
void menu::HandleMenuElements() noexcept {

    ImGui::Begin(("##main"), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings /*| ImGuiWindowFlags_NoResize*/);
    {
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
            if (ImGui::IsKeyPressed(ImGuiKey_S)) {
                bPressedSave = true;
                bWarningMethod = true;
            }
        }

        if (bPressedSave && cfg::configID != -1)
            SaveWarning(bPressedSave, bWarningMethod);

        static float lmao = 0.f;
        DrawBackgroundTexture(lmao += ImGui::GetIO().DeltaTime);
        menu::DrawCustomChildRounding(("##Tabs"), ImVec2(120, ImGui::GetContentRegionAvail().y), true, 0, ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotLeft);
        {
            float save = ImGui::GetCursorPosY();
            ImGui::PushFont(weaponIcons);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 32.f, save + 19.f));
            ImGui::Text(("R#1"));
            ImGui::PopFont();

            ImGui::PushFont(tabIcons);
            ImGui::SetCursorPosY(save + 100);
            if (menu::ButtonCenteredOnLine(("c"), 0.5f, iSelectedTab == RAGE_TAB))
                iSelectedTab = RAGE_TAB;
            if (menu::ButtonCenteredOnLine(("f"), 0.5f, iSelectedTab == ANTIAIM_TAB))
                iSelectedTab = ANTIAIM_TAB;
            if (menu::ButtonCenteredOnLine(("g"), 0.5f, iSelectedTab == VISUAL_TAB))
                iSelectedTab = VISUAL_TAB;
            if (menu::ButtonCenteredOnLine(("a"), 0.5f, iSelectedTab == MISC_TAB))
                iSelectedTab = MISC_TAB;
            if (menu::ButtonCenteredOnLine(("e"), 0.5f, iSelectedTab == SKIN_TAB))
                iSelectedTab = SKIN_TAB;
            if (menu::ButtonCenteredOnLine(("b"), 0.5f, iSelectedTab == PLAYERLST))
                iSelectedTab = PLAYERLST;
            if (menu::ButtonCenteredOnLine(("d"), 0.5f, iSelectedTab == CONFIG_TAB))
                iSelectedTab = CONFIG_TAB;
            ImGui::PopFont();
        }
        ImGui::EndChild();

        ImGui::SameLine();
        ImVec2 savedCursorPosition = ImGui::GetCursorPos();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.f, 10.f));

        ImGui::Spacing();
        ImGui::SetCursorPos(ImVec2(savedCursorPosition.x - ImGui::GetStyle().WindowPadding.x + 1, savedCursorPosition.y + ImGui::GetWindowHeight() - 25));
        menu::DrawCustomChildRounding(("##BotmBar"), ImVec2(ImGui::GetContentRegionAvail().x, 25), true, 0, ImDrawCornerFlags_BotRight);
        {
            ImGui::PushStyleColor(ImGuiCol_Text, clr::text_off);
#ifdef _DEBUG
            ImGui::Text("Developer");
#endif
#ifdef ALPHA
            ImGui::Text("Alpha");
#endif
#ifdef RELEASE
            ImGui::Text("User");
#endif
            ImGui::SameLine();
            menu::Text(("2023 - RyzeX"), 1.0f);
            ImGui::PopStyleColor();
        }
        ImGui::EndChild();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.f, 2.f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.f);
        ImGui::SetCursorPos(savedCursorPosition);
        switch (iSelectedTab) {

        case RAGE_TAB: Rage(savedCursorPosition);
            break;

        case ANTIAIM_TAB: AntiAim(savedCursorPosition);
            break;

        case VISUAL_TAB: Visual(savedCursorPosition);
            break;

        case MISC_TAB: Misc(savedCursorPosition);
            break;

        case SKIN_TAB: Skins(savedCursorPosition);
            break;

        case CONFIG_TAB: Config(savedCursorPosition);
            break;

        case PLAYERLST: PlayerList(savedCursorPosition);
            break;
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(4);

    }
    ImGui::End();
}

void menu::Rage(ImVec2 savedCursorPosition) {

    using namespace cfg::rage;

    static int iSelect = PISTOL;
    static const char* arrHitboxNames[] = {("Head"), ("Upper chest"), ("Lower chest"), ("Stomach"), ("Arms"), ("Legs")};
    static const char* arrConditionNames[] = { ("Between shots"), ("In air") };

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().WindowPadding.x + 1);
    menu::DrawCustomChildRounding(("##TopBar"), ImVec2(ImGui::GetContentRegionAvail().x, 80), true, 0, ImDrawCornerFlags_TopRight);
    {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().WindowPadding.y);
        ImGui::PushFont(weaponIcons);
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
            ImVec2 size = ImVec2((ImGui::GetContentRegionAvail().x - 5 * 9) / 7.f, ImGui::GetContentRegionAvail().y + ImGui::GetStyle().WindowPadding.y);
            if (ImGui::Button(("A##2"), size, iSelect == PISTOL))
                iSelect = PISTOL;
            ImGui::SameLine();

            if (ImGui::Button(("B##2"), size, iSelect == HEAVY_PISTOL))
                iSelect = HEAVY_PISTOL;
            ImGui::SameLine();

            if (ImGui::Button(("F##2"), size, iSelect == SCOUT))
                iSelect = SCOUT;
            ImGui::SameLine();

            if (ImGui::Button(("C##2"), size, iSelect == AWP))
                iSelect = AWP;
            ImGui::SameLine();

            if (ImGui::Button(("D##2"), size, iSelect == AUTO))
                iSelect = AUTO;
            ImGui::SameLine();

            if (ImGui::Button(("E##2"), size, iSelect == OTHER))
                iSelect = OTHER;

			ImGui::SameLine();
			if (ImGui::Button(("G##2"), size, iSelect == ZEUS))
				iSelect = ZEUS;

            ImGui::PopFont();

            ImGui::SameLine();
            ImGui::PushFont(tabIconsPreview);
            {
                ImGui::PushStyleColor(ImGuiCol_Text, clr::text_preview);
                menu::Text("c", 0.9f, true);
                ImGui::PopStyleColor();
            }
            ImGui::PopFont();

            ImGui::PopStyleVar();
        }
    }
    ImGui::EndChild();

    ImVec2 Padding = ImVec2(ImGui::GetStyle().WindowPadding.x, ImGui::GetStyle().WindowPadding.y);
    ImGui::SetCursorPos(ImVec2(savedCursorPosition.x, savedCursorPosition.y + 80 + ImGui::GetStyle().WindowPadding.y));

    ImVec2 TopLeftSize = ImVec2(ImGui::GetContentRegionAvail().x / 2 - Padding.x, ImGui::GetContentRegionAvail().y / 2 - Padding.y - 25.f);
    ImGui::BeginChild(("##TopLeft"), ImVec2(ImGui::GetContentRegionAvail().x / 2 - Padding.x, ImGui::GetContentRegionAvail().y / 2 - Padding.y - 25.f), true);
    {
        ImGui::Checkbox(("Enabled##0"), &bEnable);
        ImGui::Keybind(("aimbotkey"), &iAimbotKey);
        ImGui::Checkbox(("Silent"), &bSilentAim);
        ImGui::SliderInt(("Fov"), &iAimbotFov, 0, 180); 
        ImGui::Checkbox(("Resolver"), &bResolver);

        ImGui::Checkbox(("Force baim"), &bForceBaim);
        ImGui::Keybind(("fbaimKey"), &iForceBaimKey);

        ImGui::Checkbox(("Override"), &bOverride);
        ImGui::Keybind(("Overridekey"), &iOverrideBind);

        ImGui::Text(("Misc:"));
        ImGui::Checkbox(("Auto Grenade"), &bAutoNade);
        ImGui::Keybind(("autonade key"), &iAutoNadeBind);
        if (bAutoNade == true) {
            ImGui::SliderInt(("Minimum dmg"), &iAutoNadeMinDmg, 0, 98);
        }


        ImGui::Text(("Optimization:"));
        ImGui::Checkbox(("Limit scan"), &bLimitScan);
    }
    ImGui::EndChild();

    ImGui::SetCursorPos(ImVec2(savedCursorPosition.x, savedCursorPosition.y + 80 + TopLeftSize.y + Padding.y * 2));

    ImGui::BeginChild(("##BotLeft"), ImVec2(ImGui::GetContentRegionAvail().x / 2 - Padding.x, ImGui::GetContentRegionAvail().y - Padding.y - 25.f), true);
    {
        ImGui::Checkbox(("Doubletap"), &bDoubletap);
        ImGui::Keybind(("##iDoubletapKey"), &iDoubletapKey);

        ImGui::Checkbox(("Hideshot"), &bHideshot);
        ImGui::Keybind(("##iHideShotKey"), &iHideShotKey);
    }
    ImGui::EndChild();

    ImGui::SetCursorPos(ImVec2(savedCursorPosition.x + TopLeftSize.x + Padding.x, savedCursorPosition.y + 80 + ImGui::GetStyle().WindowPadding.y));

    ImGui::BeginChild(("##RightWhole"), ImVec2(ImGui::GetContentRegionAvail().x - Padding.x, ImGui::GetContentRegionAvail().y - Padding.y - 25.f), true);
    {
        ImGui::MultiComboBox(("Hitbox"), arrHitboxNames, bHitboxes[0][iSelect], IM_ARRAYSIZE(arrHitboxNames));
        ImGui::SliderInt(("Hitchance"), &iHitchances[iSelect], 0, 100);
        ImGui::SliderInt(("Damage"), &iMinDamages[iSelect], 0, 110);
        ImGui::SliderInt(("Override##2"), &iOverride[iSelect], 0, 110);
		ImGui::SliderInt(("Acc boost"), &iAccuracyBoost[iSelect], 0, 100);

		ImGui::MultiComboBox(("Pointscale"), arrHitboxNames, bHitboxes[1][iSelect], IM_ARRAYSIZE(arrHitboxNames));
        ImGui::SliderInt(("HeadScale"), &iHeadPoints[iSelect], 0, 100);
        ImGui::SliderInt(("BodyScale"), &iBodyPoints[iSelect], 0, 100);

        ImGui::Checkbox("Safepoint", &bSafePoint[iSelect]);
        if (bSafePoint[iSelect]) {
            ImGui::MultiComboBox(("Force safe"), arrHitboxNames, bHitboxes[2][iSelect], IM_ARRAYSIZE(arrHitboxNames));
        }

        ImGui::Checkbox(("Auto stop"), &bAutostop[iSelect]);
        if (bAutostop[iSelect]) {
            bool* bConditionsarray[] = { &bConditions[iSelect][0], &bConditions[iSelect][1] };
            ImGui::MultiComboBox(("Condition"), arrConditionNames, *bConditionsarray, IM_ARRAYSIZE(arrConditionNames));
            static const char* autoStopPhases[] = { "Disabled", "Slight", "Normal", "Aggressive" };
            ImGui::Combo("Predict", &bAutostopAggressiveness[iSelect], autoStopPhases, IM_ARRAYSIZE(autoStopPhases));
        }
        if (iSelect < 3) {

            ImGui::Checkbox(("Auto scope"), &bAutoScope[iSelect]);
        }
    }
    ImGui::EndChild();
}

void menu::AntiAim(ImVec2 savedCursorPosition) {

    using namespace cfg::antiaim;

    static int iSelect = 0;
    static const char* arrPitches[] = { ("Off"), ("Up"), ("Zero"), ("Down") };
    static const char* arrYawBases[] = { ("Local view"), ("At target") };
    static const char* arrYaws[] = { ("Forward"), ("Backward") };
    static const char* arrModifiers[] = { ("Off"), ("Jitter"), ("Random"), ("Rytter"), ("Spin")};
    static const char* arrDesyncs[] = { ("Off"), ("Static"), ("Extended"), ("Jitter"), ("Flick") };
    static const char* arrFreestands[] = { ("Off"), ("Circular"), ("Predictive")};
    static const char* arrFakelagType[] = { ("Normal"), ("Adaptive"), ("Jitter"), ("Random")};

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().WindowPadding.x + 1);
    menu::DrawCustomChildRounding(("##TopBar"), ImVec2(ImGui::GetContentRegionAvail().x, 80), true, 0, ImDrawCornerFlags_TopRight);
    {
        ImGui::PushFont(defaultFontBigger);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().WindowPadding.y);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);

        ImVec2 size = ImVec2((ImGui::GetContentRegionAvail().x - 5 * 9) / 4.f, ImGui::GetContentRegionAvail().y + ImGui::GetStyle().WindowPadding.y);
        if (ImGui::Button(("Standing"), size, iSelect == STANDING))
            iSelect = STANDING;
        ImGui::SameLine();

        if (ImGui::Button(("Moving"), size, iSelect == MOVING))
            iSelect = MOVING;
        ImGui::SameLine();

        if (ImGui::Button(("Jumping"), size, iSelect == INAIR))
            iSelect = INAIR;

        ImGui::PopFont();
        ImGui::SameLine();
        ImGui::PushFont(tabIconsPreview);
        {
            ImGui::PushStyleColor(ImGuiCol_Text, clr::text_preview);
            menu::Text("f", 1.f, true);
            ImGui::PopStyleColor();
        }
        ImGui::PopFont();

        ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImVec2 Padding = ImVec2(ImGui::GetStyle().WindowPadding.x, ImGui::GetStyle().WindowPadding.y);
    ImGui::SetCursorPos(ImVec2(savedCursorPosition.x, savedCursorPosition.y + 80 + ImGui::GetStyle().WindowPadding.y));

    ImVec2 TopLeftSize = ImVec2(ImGui::GetContentRegionAvail().x / 2.f - Padding.x, ImGui::GetContentRegionAvail().y - Padding.y - 25.f);
    ImGui::BeginChild(("##LeftWhole"), ImVec2(ImGui::GetContentRegionAvail().x / 2.f - Padding.x, ImGui::GetContentRegionAvail().y - Padding.y - 25.f), true);
    {
        ImGui::Checkbox(("Enable"), &bEnabled[iSelect]);
        if ((bEnabled[STANDING] && iSelect == STANDING) || (bEnabled[MOVING] && iSelect == MOVING) || (bEnabled[INAIR] && iSelect == INAIR)) {

            ImGui::Combo(("Pitch"), &iPitch[iSelect], arrPitches, IM_ARRAYSIZE(arrPitches));
            ImGui::Combo(("Base"), &iYawBase[iSelect], arrYawBases, IM_ARRAYSIZE(arrYawBases));
            ImGui::Combo(("Yaw"), &iYaw[iSelect], arrYaws, IM_ARRAYSIZE(arrYaws));
            ImGui::Checkbox("Enable Manual Anti-Aim", &cfg::antiaim::bManualAA);
            if (cfg::antiaim::bManualAA)
            {
                //i dunno why it doesnt but the keybind into a new lne
                ImGui::Checkbox(("Left"), &bManualAAL);
                ImGui::Keybind("##iManualAAL", &iManualAAL);
                ImGui::Checkbox(("Right"), &bManualAAR);
                ImGui::Keybind("##iManualAAR", &iManualAAR);
            }
            ImGui::Combo(("Modifier"), &iModifier[iSelect], arrModifiers, IM_ARRAYSIZE(arrModifiers));
            if (iModifier[iSelect] != 0) {
				if (iModifier[iSelect] == 3) {

					if (ImGui::Button(("Add way##1"), ImVec2(ImGui::GetContentRegionAvail().x / 2, 20), true, true) && iEnabledJitters[iSelect] < vecJitterWays[iSelect].size() - 1)
						iEnabledJitters[iSelect]++;
					ImGui::SameLine();
					if (ImGui::Button(("Remove way##1"), ImVec2(ImGui::GetContentRegionAvail().x, 20), true, true) && iEnabledJitters[iSelect] > 0)
						iEnabledJitters[iSelect]--;

					for (size_t i = 0; i < iEnabledJitters[iSelect]; i++)
						ImGui::SliderInt(std::format("Step {}", i).c_str(), &vecJitterWays[iSelect].at(i), -180, 180);
				}
				else if (iModifier[iSelect] == 4) {

					ImGui::SliderInt(("Speed"), &iSpinSpeed[iSelect], 0, 90);
				}
				else {
					ImGui::SliderInt(("Value"), &iJitterValue[iSelect], 0, 90);
				}
				ImGui::Checkbox(("Anti Prediction"), &bAntiJitter[iSelect]);
			}
            ImGui::Combo(("Desync"), &iDesyncType[iSelect], arrDesyncs, IM_ARRAYSIZE(arrDesyncs));
            if (iDesyncType[iSelect] != 0 && iDesyncType[iSelect] != 5) {
                ImGui::Checkbox(("Inverter"), &bInverter);
                ImGui::Keybind(("##iInverterBind"), &iInverterBind);
                if (iDesyncType[iSelect] == 4) {
                    ImGui::SliderInt(("Offset"), &iFlickOffset[iSelect], -90, 90);
                    ImGui::SliderInt(("Switch tick"), &flickAngleSwitch[iSelect], 0, iFakeLagMax);
                }
            }
            ImGui::Checkbox(("Invert on shot"), &bInvertOnShoot[iSelect]);
            ImGui::Combo(("Freestand"), &iFreestand[iSelect], arrFreestands, IM_ARRAYSIZE(arrFreestands));
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();
    ImVec2 NextWindowCursor = ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPosY() + ImGui::GetContentRegionAvail().y / 2.f - 25.f);

    ImGui::BeginChild(("##TopRight"), ImVec2(ImGui::GetContentRegionAvail().x - Padding.x, ImGui::GetContentRegionAvail().y / 2.f - Padding.y - 25.f), true);
    {
        ImGui::Checkbox(("Fakelag"), &bFakelag);
        if (bFakelag) {

            iFakelagMin = std::clamp(iFakelagMin, 0, iFakelag);
            iFakeLagMax = std::clamp(iFakeLagMax, iFakelagMin, iFakelag);

            ImGui::Combo(("Type"), &iFakeLagType, arrFakelagType, IM_ARRAYSIZE(arrFakelagType));
            ImGui::SliderInt(("Base"), &iFakelag, 0, 14);
            ImGui::SliderInt(("Min"), &iFakelagMin, 0, iFakelag);
            ImGui::SliderInt(("Max"), &iFakeLagMax, iFakelagMin, iFakelag);

            ImGui::Checkbox(("Break lagcompenstaion"), &bBreakLagcompensation);

            ImGui::Checkbox(("Fake ping"), &cfg::misc::bFakePing);
            if (cfg::misc::bFakePing)
                ImGui::SliderFloat(("Value"), &cfg::misc::flFakePingFactor, 0.f, 1000.f, ("%.f"));
        }
    }
    ImGui::EndChild();

    ImGui::SetCursorPos(NextWindowCursor);
    ImGui::BeginChild(("##BotRight"), ImVec2(ImGui::GetContentRegionAvail().x - Padding.x, ImGui::GetContentRegionAvail().y - Padding.y - 25.f), true);
    {
        ImGui::Checkbox(("Slow walk"), &bFakeWalk);
        ImGui::Keybind(("##iFakeWalkKey"), &iFakeWalkKey);
        if (bFakeWalk)
        ImGui::SliderFloat(("Speed"), &iFakeWalkSpeed, 0, 100, ("%.f"));
        ImGui::Checkbox(("Fakeduck"), &bFakeDuck);
        ImGui::Keybind(("##iFakeDuckKey"), &iFakeDuckKey);

        ImGui::Checkbox("Auto peek", &bAutoPeek);
        ImGui::ColorEdit4("##localIdealTickColor", cfg::model::flAutopeekColor, true);
        ImGui::Keybind("##iAutoPeek", &iAutoPeek);
    }
    ImGui::EndChild();
}

void menu::Visual(ImVec2 savedCursorPosition) {

    using namespace cfg::visual;

    static int iSelect = 0;
    static const char* arrFlagNames[] = {("Name"), ("Health"), ("Armor"), ("Ammo"), ("Money"), ("Weapon"), ("Resolver"), ("Fakeduck"), ("Aimbot")};
    static const char* arrChamsType[] = { ("Default"), ("Flat"), ("Glow"), ("Thin glow"), ("Animated") };
    static const char* arrMaterialType[] = { ("Default"), ("Flat") };
    static const char* arrSkyboxes[] = { "None", "Baggage", "Tibet", "Vietnam",  "Lunacy", "Embassy", "Italy", "Jungle", "Office", "Daylight1", "Daylight2", "Daylight3", "Daylight4", "Day", "Nuke", "Dust", "Venice", "Cloudy", "Night1", "Night2", "Vertigo", "VertigoHdr", "SkyDust", "Aztec"  };
    
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().WindowPadding.x + 1);
    menu::DrawCustomChildRounding(("##TopBar"), ImVec2(ImGui::GetContentRegionAvail().x, 80), true, 0, ImDrawCornerFlags_TopRight);
    {
        ImGui::PushFont(defaultFontBigger);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().WindowPadding.y);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);

        ImVec2 size = ImVec2((ImGui::GetContentRegionAvail().x - 5 * 9) / 4.f, ImGui::GetContentRegionAvail().y + ImGui::GetStyle().WindowPadding.y);
        if (ImGui::Button(("Enemy"), size, iSelect == 0))
            iSelect = 0;
        ImGui::SameLine();

        if (ImGui::Button(("Team"), size, iSelect == 1))
            iSelect = 1;
        ImGui::SameLine();

        if (ImGui::Button(("Local"), size, iSelect == 2))
            iSelect = 2;

        ImGui::PopFont();
        ImGui::SameLine();
        ImGui::PushFont(tabIconsPreview);
        {
            ImGui::PushStyleColor(ImGuiCol_Text, clr::text_preview);
            menu::Text("g", 1.f, true);
            ImGui::PopStyleColor();
        }
        ImGui::PopFont();

        ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImVec2 Padding = ImVec2(ImGui::GetStyle().WindowPadding.x, ImGui::GetStyle().WindowPadding.y);
    ImGui::SetCursorPos(ImVec2(savedCursorPosition.x, savedCursorPosition.y + 80 + ImGui::GetStyle().WindowPadding.y));

    ImVec2 TopLeftSize = ImVec2(ImGui::GetContentRegionAvail().x / 2.f - Padding.x, ImGui::GetContentRegionAvail().y * 0.65f - Padding.y - 25.f);
    ImGui::BeginChild(("##LeftTop"), ImVec2(ImGui::GetContentRegionAvail().x / 2.f - Padding.x, ImGui::GetContentRegionAvail().y * 0.65f - Padding.y - 25.f), true);
    {
        using namespace cfg::visual;
        ImGui::Checkbox(("Enable##0"), &bEnable[iSelect]);
        ImGui::Checkbox(("Bounding box"), &bBox[iSelect]);
        ImGui::ColorEdit4(("##flBoxColor"), flBoxColor[iSelect]);

        ImGui::Checkbox(("Health bar"), &bHealth[iSelect]);
        ImGui::ColorEdit4(("##flHealthColorStart"), flHealthColorStart[iSelect], true);
        ImGui::ColorEdit4(("##flHealthColorEnd"), flHealthColorEnd[iSelect]);

        ImGui::Checkbox(("Name"), &bName[iSelect]);
        ImGui::ColorEdit4(("##flNameColor"), flNameColor[iSelect]);

        ImGui::Checkbox(("Weapon"), &bWeapon[iSelect]);
        ImGui::ColorEdit4(("##flWeaponColor"), flWeaponColor[iSelect]);

        ImGui::Checkbox(("Ammo"), &bAmmo[iSelect]);
        ImGui::ColorEdit4(("##flAmmoColor"), flAmmoColor[iSelect]);

        ImGui::Checkbox(("Glow"), &bGlow[iSelect]);
        ImGui::ColorEdit4(("##flGlowColor"), flGlowColor[iSelect]);

        ImGui::Checkbox(("Skeleton"), &bSkeleton[iSelect]);
        ImGui::ColorEdit4(("##flSkeletonColor"), flSkeletonColor[iSelect]);

        ImGui::Checkbox(("Bullet tracer"), &bBulletTracer[iSelect]);
        ImGui::ColorEdit4(("##flBulletTracerColor"), flBulletTracerColor[iSelect]);

        if (iSelect == ENEMY) {

            ImGui::Checkbox(("Out of fov"), &cfg::misc::bOOF);
            if (cfg::misc::bOOF) {
                ImGui::ColorEdit4(("##flOOF"), cfg::misc::flOOFColor);
                ImGui::SliderInt(("Distance##2"), &cfg::misc::iOOFDistance, 1, 100);
                ImGui::SliderInt(("Size"), &cfg::misc::iOOFSize, 1, 30);
            }
        }

        ImGui::MultiComboBox(("Flags"), arrFlagNames, bFlags[iSelect], flFlagsColor[iSelect], IM_ARRAYSIZE(arrFlagNames));
    }
    ImGui::EndChild();

    ImGui::SetCursorPos(ImVec2(savedCursorPosition.x, savedCursorPosition.y + 80 + ImGui::GetStyle().WindowPadding.y + TopLeftSize.y + Padding.y));
    ImGui::BeginChild(("##LeftBot"), ImVec2(ImGui::GetContentRegionAvail().x / 2.f - Padding.x, ImGui::GetContentRegionAvail().y - Padding.y - 25.f), true);
    {
        using namespace cfg::model;
        static const char* arrChamsConfig[] = { "Player", "Overlays", "Attachment", "Viewmodel", "Weapon", "Sleeve", "Props"};
        static int iSelectedConfig = 0;

        static const char* iOverlayTypes[] = { ("Glow"), ("Thin"), ("Animated") };
        static int iSelectedOverlay = 0;

        if (iSelect == LOCAL)
            ImGui::Combo(("Chams##2"), &iSelectedConfig, arrChamsConfig, IM_ARRAYSIZE(arrChamsConfig));
        else {
            iSelectedConfig = iSelectedConfig > 2 ? 2 : iSelectedConfig;
            ImGui::Combo(("Chams##2"), &iSelectedConfig, arrChamsConfig, IM_ARRAYSIZE(arrChamsConfig) - 4);
        }
        switch (iSelectedConfig) {

        case 0:
            // normal
            ImGui::Combo(("Material"), &iType[iSelect], arrMaterialType, IM_ARRAYSIZE(arrMaterialType));
            ImGui::Checkbox(("Player"), &bChams[iSelect]);
            ImGui::ColorEdit4(("##ChamsColor"), ChamsColor[iSelect]);

            ImGui::Checkbox(("Player behind wall"), &bChamsXQZ[iSelect]);
            ImGui::ColorEdit4(("##ChamsColorXQZ"), ChamsColorXQZ[iSelect]);


            if (iSelect == ENEMY) {

                ImGui::Checkbox(("Backtrack"), &bBacktrackChams);
                if (bBacktrackChams) {
                    ImGui::ColorEdit4(("##enemyBTColor"), flBacktrackColor);
                    ImGui::Combo(("Material##5"), &iBacktrackType, arrChamsType, IM_ARRAYSIZE(arrChamsType));
                }
            }
            else if (iSelect == LOCAL) {

                ImGui::Checkbox(("Desync"), &bDesyncChams);
                if (bDesyncChams) {
                    ImGui::ColorEdit4(("##localDesyncColor"), flDesyncChamsCol);
                    ImGui::Combo(("Material##4"), &bDesyncType, arrChamsType, IM_ARRAYSIZE(arrChamsType));
                }
            }

            break;

        case 1:
            ImGui::Combo(("Overlay##1337"), &iSelectedOverlay, iOverlayTypes, IM_ARRAYSIZE(iOverlayTypes));
            switch (iSelectedOverlay) {
            case 0:
                // overlay
                ImGui::Checkbox(("Overlay##1"), &bOverlay[iSelect]);
                ImGui::ColorEdit4(("##OverlayColor"), OverlayColor[iSelect]);

                ImGui::Checkbox(("Overlay behind wall##1"), &bOverlayXQZ[iSelect]);
                ImGui::ColorEdit4(("##OverlayColorXQZ"), OverlayColorXQZ[iSelect]);
                break;

            case 1:
                // thin
                ImGui::Checkbox(("Overlay##2"), &bThinOverlay[iSelect]);
                ImGui::ColorEdit4(("##ThinOverlayColor"), ThinOverlayColor[iSelect]);

                ImGui::Checkbox(("Overlay behind wall##2"), &bThinOverlayXQZ[iSelect]);
                ImGui::ColorEdit4(("##ThinOverlayColorXQZ"), ThinOverlayColorXQZ[iSelect]);
                break;

            case 2:
                // animated
                ImGui::Checkbox(("Overlay##3"), &bAnimOverlay[iSelect]);
                ImGui::ColorEdit4(("##AnimOverlayColor"), AnimOverlayColor[iSelect]);

                ImGui::Checkbox(("Overlay behind wall##3"), &bAnimOverlayXQZ[iSelect]);
                ImGui::ColorEdit4(("##AnimOverlayColorXQZ"), AnimOverlayColorXQZ[iSelect]);
                break;
            }


            break;

        case 2:

            ImGui::Combo(("Material"), &iAttachmentMaterial[iSelect], arrMaterialType, IM_ARRAYSIZE(arrMaterialType));
            ImGui::Checkbox("Attachment", &bAttachment[iSelect]);
            ImGui::ColorEdit4("##attachmentpicker", flAttachmentColor[iSelect]);
            ImGui::Checkbox("Wireframe", &bAttachmentXhair[iSelect]);

            ImGui::Combo(("Overlay##1337"), &iSelectedOverlay, iOverlayTypes, IM_ARRAYSIZE(iOverlayTypes));
            switch (iSelectedOverlay) {
            case 0:
                // overlay
                ImGui::Checkbox(("Overlay##1"), &bAttachmentOverlay[iSelect]);
                ImGui::ColorEdit4(("##OverlayColor"), bAttachmentOverlayColor[iSelect]);
                ImGui::Checkbox(("Wireframe##3"), &bAttachmentOverlayXhair[iSelect]);
                break;

            case 1:
                // thin
                ImGui::Checkbox(("Overlay##2"), &bAttachmentThinOverlay[iSelect]);
                ImGui::ColorEdit4(("##ThinOverlayColor"), bAttachmentThinOverlayColor[iSelect]);
                ImGui::Checkbox(("Wireframe##3"), &bAttachmentThinOverlayXhair[iSelect]);
                break;

            case 2:
                // animated
                ImGui::Checkbox(("Overlay##3"), &bAttachmentAnimatedOverlay[iSelect]);
                ImGui::ColorEdit4(("##AnimOverlayColor"), bAttachmentAnimatedOverlayColor[iSelect]);
                ImGui::Checkbox(("Wireframe##3"), &bAttachmentAnimatedOverlayXhair[iSelect]);
                break;
            }
            break;

        case 3:

            ImGui::Combo(("Material"), &iViewmodelType, arrMaterialType, IM_ARRAYSIZE(arrMaterialType));

            ImGui::Checkbox("Hands", &bViewmodel);
            ImGui::ColorEdit4("##viewmodelColor", flViewmodelColor);
            ImGui::Combo(("Overlay##1337"), &iSelectedOverlay, iOverlayTypes, IM_ARRAYSIZE(iOverlayTypes));
            switch (iSelectedOverlay) {
            case 0:
                // overlay
                ImGui::Checkbox(("Overlay##1"), &bViewmodelOverlay);
                ImGui::ColorEdit4(("##OverlayColor"), flViewmodelOverlayColor);
                ImGui::Checkbox(("Wireframe##3"), &bViewmodelOverlayXhair);
                break;

            case 1:
                // thin
                ImGui::Checkbox(("Overlay##2"), &bViewmodelThinOverlay);
                ImGui::ColorEdit4(("##ThinOverlayColor"), flViewmodelThinOverlayColor);
                ImGui::Checkbox(("Wireframe##3"), &bViewmodelThinOverlayXhair);
                break;

            case 2:
                // animated
                ImGui::Checkbox(("Overlay##3"), &bViewmodelAnimOverlay);
                ImGui::ColorEdit4(("##AnimOverlayColor"), flViewmodelAnimOverlayColor);
                ImGui::Checkbox(("Wireframe##3"), &bViewmodelAnimOverlayXhair);
                break;
            }

            break;

        case 4:

            ImGui::Combo(("Material"), &iWeaponType, arrMaterialType, IM_ARRAYSIZE(arrMaterialType));

            ImGui::Checkbox("Weapon", &cfg::model::bWeapon);
            ImGui::ColorEdit4("##weaponColor", cfg::model::flWeaponColor);
            ImGui::Combo(("Overlay##1337"), &iSelectedOverlay, iOverlayTypes, IM_ARRAYSIZE(iOverlayTypes));
            switch (iSelectedOverlay) {
            case 0:
                // overlay
                ImGui::Checkbox(("Overlay##1"), &bWeaponOverlay);
                ImGui::ColorEdit4(("##OverlayColor"), flWeaponOverlayColor);
                ImGui::Checkbox(("Wireframe##3"), &bWeaponOverlayXhair);
                break;

            case 1:
                // thin
                ImGui::Checkbox(("Overlay##2"), &bWeaponThinOverlay);
                ImGui::ColorEdit4(("##ThinOverlayColor"), bWeaponThinOverlayColor);
                ImGui::Checkbox(("Wireframe##3"), &bWeaponThinOverlayXhair);
                break;

            case 2:
                // animated
                ImGui::Checkbox(("Overlay##3"), &bWeaponAnimOverlay);
                ImGui::ColorEdit4(("##AnimOverlayColor"), flWeaponAnimOverlayColor);
                ImGui::Checkbox(("Wireframe##3"), &flWeaponAnimOverlayXhair);
                break;
            }

            break;

        case 5:

            ImGui::Combo(("Material"), &iSleeveType, arrMaterialType, IM_ARRAYSIZE(arrMaterialType));

            ImGui::Checkbox("Sleeve", &bSleeve);
            ImGui::ColorEdit4("##SleeveColor", flSleeveColor);
            ImGui::Combo(("Overlay##1337"), &iSelectedOverlay, iOverlayTypes, IM_ARRAYSIZE(iOverlayTypes));
            switch (iSelectedOverlay) {
            case 0:
                // overlay
                ImGui::Checkbox(("Overlay##1"), &bSleeveOverlay);
                ImGui::ColorEdit4(("##OverlayColor"), flSleeveOverlayColor);
                ImGui::Checkbox(("Wireframe##3"), &bSleeveOverlayXhair);
                break;

            case 1:
                // thin
                ImGui::Checkbox(("Overlay##2"), &bSleeveThinOverlay);
                ImGui::ColorEdit4(("##ThinOverlayColor"), flSleeveThinOverlayColor);
                ImGui::Checkbox(("Wireframe##3"), &bSleeveThinOverlayXhair);
                break;

            case 2:
                // animated
                ImGui::Checkbox(("Overlay##3"), &bSleeveAnimOverlay);
                ImGui::ColorEdit4(("##AnimOverlayColor"), flSleeveAnimOverlayColor);
                ImGui::Checkbox(("Wireframe##3"), &bSleeveAnimOverlayXhair);
                break;
            }

            break;

        case 6:

            ImGui::Combo(("Material"), &iPropsType, arrMaterialType, IM_ARRAYSIZE(arrMaterialType));

            ImGui::Checkbox("Props", &bProps);
            ImGui::ColorEdit4("##PropsColor", flPropsColor);
            ImGui::Combo(("Overlay##1337"), &iSelectedOverlay, iOverlayTypes, IM_ARRAYSIZE(iOverlayTypes));
            switch (iSelectedOverlay) {
            case 0:
                // overlay
                ImGui::Checkbox(("Overlay##1"), &bPropsOverlay);
                ImGui::ColorEdit4(("##OverlayColor"), flPropsOverlayColor);
                ImGui::Checkbox(("Wireframe##3"), &bPropsOverlayXhair);
                break;

            case 1:
                // thin
                ImGui::Checkbox(("Overlay##2"), &bPropsThinOverlay);
                ImGui::ColorEdit4(("##ThinOverlayColor"), flPropsThinOverlayColor);
                ImGui::Checkbox(("Wireframe##3"), &bPropsThinOverlayXhair);
                break;

            case 2:
                // animated
                ImGui::Checkbox(("Overlay##3"), &bPropsAnimOverlay);
                ImGui::ColorEdit4(("##AnimOverlayColor"), flPropsAnimOverlayColor);
                ImGui::Checkbox(("Wireframe##3"), &bPropsAnimOverlayXhair);
                break;
            }

            break;
        }

        //static const char* arrNames[] = { "Normal", "Behind wall", "Overlay", "Overlay behind wall", "Thin", "Thin behind wall", "Animated", "Animated behind wall" };
        //bool bWireFrames[] = { &bXhair[iSelect], &bXhair[iSelect], &bOverlayXhair[iSelect], &bOverlayXhairXQZ[iSelect], &bThinOverlayXhair[iSelect], &bThinOverlayXhairXQZ[iSelect], &bAnimOverlayXhair[iSelect], &bAnimOverlayXhairXQZ[iSelect] };
        
        //ImGui::MultiComboBox("Wireframe##selectables", arrNames, *bWireFrames, IM_ARRAYSIZE(bWireFrames));
    }
    ImGui::EndChild();

    ImGui::SetCursorPos(ImVec2(savedCursorPosition.x + TopLeftSize.x + Padding.x, savedCursorPosition.y + 80 + ImGui::GetStyle().WindowPadding.y));
    ImVec2 TopRightSize = ImVec2(ImGui::GetContentRegionAvail().x - Padding.x, ImGui::GetContentRegionAvail().y * 0.4f - Padding.y - 25.f);
    ImGui::BeginChild(("##RightTop"), ImVec2(ImGui::GetContentRegionAvail().x - Padding.x, ImGui::GetContentRegionAvail().y * 0.4f - Padding.y - 25.f), true);
    {
        using namespace cfg::misc;
        using namespace cfg::visual;

        ImGui::Checkbox(("Always draw viewmodel"), &bOnScopeViewmodel);

        ImGui::Checkbox(("Dropped weapons"), &bDroppedWeaponESP);
        ImGui::ColorEdit4(("##flDroppedWeaponESP"), flDroppedWeaponESP);

        ImGui::Checkbox(("Projectile"), &bProjectileESP);
        ImGui::ColorEdit4(("##flProjectileESP"), flProjectileESP);

        ImGui::Checkbox(("Nade prediction"), &bNadePrediction);

        ImGui::Checkbox(("Bullet Impact"), &bBulletImpact);
        ImGui::ColorEdit4(("##flImpactColor1"), flImpactColor[0], true);
        ImGui::ColorEdit4(("##flImpactColor2"), flImpactColor[1]);

        ImGui::Checkbox(("World crosshair"), &bWorldCrosshair);
        ImGui::ColorEdit4(("##flWorldCrosshairColor"), flWorldCrosshairColor);

		ImGui::Checkbox(("Damage Indicator"), &bDamageIndicator);
		ImGui::ColorEdit4(("##flDamageIndicator"), flDamageIndicator);

        ImGui::Checkbox(("Capsule on hit"), &bDrawCapsule);
        ImGui::ColorEdit4(("##flDrawCapsuleColor"), flDrawCapsuleColor, true);
        ImGui::ColorEdit4(("##flDrawCapsuleColorHit"), flDrawCapsuleColorHit);

        ImGui::Checkbox(("Model scale"), &bSkinnyBoy);
        if (bSkinnyBoy)
            ImGui::SliderInt(("Scale"), &iSkinnyBoy, 0, 100);
            
        ImGui::Checkbox(("Samurai hat"), &bHat);
        ImGui::ColorEdit4(("##flHat"), flHat);

        ImGui::Checkbox(("Custom hud"), &bCustomHud);
    }
    ImGui::EndChild();

    ImGui::SetCursorPos(ImVec2(savedCursorPosition.x + TopLeftSize.x + Padding.x, savedCursorPosition.y + 80 + ImGui::GetStyle().WindowPadding.y + TopRightSize.y + Padding.y));
    ImGui::BeginChild(("##RightBot"), ImVec2(ImGui::GetContentRegionAvail().x - Padding.x, ImGui::GetContentRegionAvail().y - 25.f - Padding.y), true);
    {       
        using namespace cfg::misc;

        ImGui::Checkbox(("Nightmode"), &bNightmode);
        ImGui::ColorEdit4(("##flNightmodeColor"), flNightmodeColor, true);
        ImGui::ColorEdit4(("##flPropColor"), flPropColor);

        // prop chams

        ImGui::Combo(("##Skybox"), &iSkybox, arrSkyboxes, IM_ARRAYSIZE(arrSkyboxes));
        ImGui::ColorEdit4(("##flSkyboxColor"), flSkyboxColor);

        ImGui::Checkbox(("Lamp color"), &bOverrideLampColors);
        ImGui::ColorEdit4(("##flLampColors"), flLampColors);
        if (bOverrideLampColors)
            ImGui::SliderInt(("Flicker"), &iFlicker, 0, 255);

        ImGui::Checkbox("Override fog", &bOverrideFog);
        if (bOverrideFog) {
            ImGui::ColorEdit4("##fog color", flFogColor);
            ImGui::SliderInt("Fog start", &iFogStart, -100, 2000);
            ImGui::SliderInt("Fog end", &iFogEnd, -100, 5000);
        }

        ImGui::Checkbox(("Aspect ratio"), &bAspectRatio);
        if (bAspectRatio)
            ImGui::SliderInt(("##Value"), &iAspectRatio, 0, 100);

        ImGui::Checkbox(("Remove smoke"), &bRemovals[0]);
        ImGui::Checkbox(("Remove flash"), &bRemovals[1]);
        ImGui::Checkbox(("Remove recoil"), &bRemovals[2]);
        ImGui::Checkbox(("Remove zoom"), &bRemovals[3]);
        ImGui::Checkbox(("Remove scope"), &bRemovals[5]);
		ImGui::Checkbox(("Remove post processing"), &bRemovals[4]);
		ImGui::Checkbox(("Remove shadows"), &bRemovals[6]);
		if (bRemovals[5])
        {
            ImGui::ColorEdit4(("##scope stuff"), flScopeColor, true);
            ImGui::ColorEdit4(("##scope stuff2"), flScopeColorEnd);
            ImGui::SliderInt(("Scope length"), &iScopeLength, 0.f, 100.f);
        }
    }
    ImGui::EndChild();
}

void menu::Misc(ImVec2 savedCursorPosition) {

    static const char* arrRadios[] = { ("2000's"), ("Rock"), ("Techno"), ("Rap"), ("Chill"), ("Club"), ("House"), ("8-Bit"), ("8-Bit Alternative"), ("Lo-Fi"), ("Eurobeat"), ("Nightcore"),("Radio 1"), ("Phonk") };
    using namespace cfg::misc;

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().WindowPadding.x + 1);
    menu::DrawCustomChildRounding(("##TopBar"), ImVec2(ImGui::GetContentRegionAvail().x, 80), true, 0, ImDrawCornerFlags_TopRight);
    {
        ImGui::SameLine();
        ImGui::PushFont(tabIconsPreview);
        {
            ImGui::PushStyleColor(ImGuiCol_Text, clr::text_preview);
            menu::Text("a", 1.f, true);
            ImGui::PopStyleColor();
        }
        ImGui::PopFont();
    }
    ImGui::EndChild();

    ImVec2 Padding = ImVec2(ImGui::GetStyle().WindowPadding.x, ImGui::GetStyle().WindowPadding.y);
    ImGui::SetCursorPos(ImVec2(savedCursorPosition.x, savedCursorPosition.y + 80 + ImGui::GetStyle().WindowPadding.y));

    ImVec2 TopLeftSize = ImVec2(ImGui::GetContentRegionAvail().x / 2.f - Padding.x, ImGui::GetContentRegionAvail().y - Padding.y - 25.f);
    ImGui::BeginChild("##LeftWhole", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - Padding.x, ImGui::GetContentRegionAvail().y - Padding.y - 25.f), true);
    {
        ImGui::Checkbox(("Thirdperson"), &bThirdPerson);
        ImGui::Keybind(("##iThirdPersonKey"), &iThirdPersonKey);
        if (bThirdPerson)
            ImGui::SliderInt(("Distance"), &iThirdPersonDistance, 0, 300);

        ImGui::SliderInt(("Viewmodel"), &iViewModelFov, 45, 140);
        ImGui::SliderInt(("Debug fov"), &iDebugFov, 40, 140);
        ImGui::Checkbox(("Bunny hop"), &bBunnyHop);
        ImGui::Checkbox(("Auto strafe"), &bAutoStrafe);
        ImGui::Checkbox(("Fast stop"), &bFastStop);
        ImGui::Checkbox(("Infinite duck"), &bInfiniteDuck);
        ImGui::Checkbox(("Blockbot"), &bBlockbot);
        ImGui::Keybind(("##iBlockbotKey"), &iBlockbotKey);
        
        ImGui::Checkbox(("Clantag"), &bClantag);
        if (bClantag) {
            char CustomClantagBuffer[32]{};
            strcpy(CustomClantagBuffer, szCustomClantagBuffer.c_str());
            ImGui::InputText(("Custom clantag"), CustomClantagBuffer, sizeof(CustomClantagBuffer));
            szCustomClantagBuffer = CustomClantagBuffer;
        }
        
        ImGui::Checkbox(("Invert knife"), &bInvertKnife);
		ImGui::Checkbox(("Persistent kill feed"), &bPreserveKillfeed);
		ImGui::Checkbox(("Animation Breaker"), &cfg::antiaim::bSlideWalk);

        ImGui::Checkbox("Motion blur", &bMotionBlur);
        if (bMotionBlur) {

            ImGui::Text("NVIDIA GPU only");
            ImGui::SliderFloat(("Strength"), &flStrength, 0, 15, "%.1f");
            ImGui::SliderFloat(("Falling"), &flFallingIntensity, 0, 10, "%.1f");
            ImGui::SliderFloat(("Rotation"), &flRotationIntensity, 0, 10, "%.1f");
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();
    ImVec2 NextWindowCursor = ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPosY() + ImGui::GetContentRegionAvail().y / 2.f - 25.f);
    ImGui::BeginChild(("##TopRight"), ImVec2(ImGui::GetContentRegionAvail().x - Padding.x, ImGui::GetContentRegionAvail().y / 2.f - Padding.y - 25.f), true);
    {
        ImGui::Checkbox(("Killsay"), &bKillsay);
        if (bKillsay) {
            char killsayBuffer[32]{};
            strcpy(killsayBuffer, szKillsayBuffer.c_str());
            ImGui::InputText(("Text"), killsayBuffer, sizeof(killsayBuffer));
            szKillsayBuffer = killsayBuffer;
        }

        char bombCharBuffer[12]{};
        strcpy(bombCharBuffer, szBombBuffer.c_str());
        ImGui::InputText(("Bomb text"), bombCharBuffer, sizeof(bombCharBuffer));
        szBombBuffer = bombCharBuffer;

        ImGui::Checkbox(("Filter logs"), &bOnlyCheatlog);

        for (size_t i = 0; i < 65; i++) {
            if (playerList::arrPlayers[i].bLocalPlayer) {
                if (playerList::arrPlayers[i].iTeamID == TEAM_TT) {

                    static const char* pistolsT[] = { ("none"), ("glock"), ("dual beretta"), ("p250"),  ("tec9 / cz-auto"), ("desert eagle / revolver") };
                    static const char* riflesT[] = { ("none"), ("galil ar"), ("ak47"), ("sg 553") };
                    static const char* snipersT[] = { ("none"), ("ssg08"), ("awp"), ("g3sg1") };
                    static const char* equipmentsT[] = { ("kevlar + helmet"), ("zeus") };

                    static const char* grenadesT[] = { ("molotov"), ("decoy grenade"),("flashbang"),("he grenade"), ("smoke grenade") };

                    ImGui::Checkbox(("Autobuy masterswitch"), &bAutobuy);

                    ImGui::Combo(("Pistols"), &cfg::misc::iPistols, pistolsT, IM_ARRAYSIZE(pistolsT));
                    ImGui::Combo(("Snipers"), &cfg::misc::iSnipers, snipersT, IM_ARRAYSIZE(snipersT));

                    ImGui::MultiComboBox(("Equipments"), equipmentsT, bEquipments, IM_ARRAYSIZE(equipmentsT));
                    ImGui::MultiComboBox(("Grenades"), grenadesT, bGrenades, IM_ARRAYSIZE(grenadesT));
                }
                else {
                    static const char* pistolsCT[] = { ("None"), ("Usp-s / p2000"), ("Dual beretta"), ("P250"), ("Five-seven / Cz-auto"), ("Desert eagle / Revolver") };
                    static const char* riflesCT[] = { ("None"), ("Famas"), ("M4a1-s / M4a4"), ("Aug") };
                    static const char* snipersCT[] = { ("None"), ("Ssg08"), ("Awp"), ("Scar-20") };
                    static const char* equipmentsCT[] = { ("Kevlar + Helmet"), ("Zeus"), ("Defuse kit") };
                    static const char* grenadesCT[] = { ("Incendiary grenade"), ("Decoy grenade"), ("Flashbang"), ("He grenade"), ("Smoke grenade") };
                   
                    ImGui::Checkbox(("Autobuy masterswitch"), &bAutobuy);

                    ImGui::Combo(("Pistols"), &cfg::misc::iPistols, pistolsCT, IM_ARRAYSIZE(pistolsCT));
                    ImGui::Combo(("Snipers"), &cfg::misc::iSnipers, snipersCT, IM_ARRAYSIZE(snipersCT));

                    ImGui::MultiComboBox(("Equipments"), equipmentsCT, bEquipments, IM_ARRAYSIZE(equipmentsCT));
                    ImGui::MultiComboBox(("Grenades"), grenadesCT, bGrenades, IM_ARRAYSIZE(grenadesCT));
                }
                break;
            }
        }
    }
    ImGui::EndChild();

    ImGui::SetCursorPos(NextWindowCursor);
    ImGui::BeginChild("##BotRight", ImVec2(ImGui::GetContentRegionAvail().x - Padding.x, ImGui::GetContentRegionAvail().y - Padding.y - 25.f), true);
    {
		ImGui::Checkbox("Keybind", &cfg::misc::bKeyBindList);
        if (bKeyBindList) {
            static const char* options[] = { "Aimbot", "Exploit", "Force baim", "DMG override", "Slow walk", "Fake duck", "Auto peek", "Thirdperson", "Blockbot", "Ping", "Auto Nade", "Manual Anti-Aim"};
            ImGui::MultiComboBox("Keybinds", options, cfg::misc::iKeyBindList, IM_ARRAYSIZE(options));
        }

        static const char* arrHitSoundOptions[] = { ("None"), ("Default"), ("Custom") };
        ImGui::Combo(("Hitsound##combo"), &iHitSound, arrHitSoundOptions, IM_ARRAYSIZE(arrHitSoundOptions));
        ImGui::SliderFloat("Volume##123", &flHitSoundVolume, 0.f, 100.f, "%.2f");
        if (iHitSound == 2) {

            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            static int soundItemCurrent1 = -1;
            static std::string soundItem;
            if (ImGui::ListBoxVector(("##soundFiles"), &soundItemCurrent1, pConfig->vecSoundFileNames, 5)) {

                soundItem = pConfig->vecSoundFileNames[soundItemCurrent1];
                cfg::misc::szWavPath = std::filesystem::path(pConfig->SoundPath.c_str() + soundItem).string();
            }

            if (ImGui::Button(("Refresh"), ImVec2(ImGui::GetContentRegionAvail().x, 20), true, true))
                pConfig->RefreshSounds();
            ImGui::PopItemWidth();
        }

        ImGui::Checkbox(("Radio"), &bEnableRadio);
        ImGui::Keybind(("##iRadioMuteHotKey"), &iRadioMuteHotKey);

        if (bEnableRadio) {
            ImGui::SliderFloat(("Volume"), &flRadioVolume, 0.f, 100.f, ("%.1f%%"));
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::ListBox(("##Radio"), &iRadioStation, arrRadios, IM_ARRAYSIZE(arrRadios));
            ImGui::PopItemWidth();
        }

        ImGui::Checkbox("Vizualizer", &cfg::vizualizer::bEnabled);
        ImGui::Checkbox("Dynamic threshold", &cfg::vizualizer::bDynamic);
        ImGui::SliderFloat("Sens", &cfg::vizualizer::flSensitivity, 0.00f, 0.1f, "%.3f");
        if (!cfg::vizualizer::bDynamic)
		    ImGui::SliderFloat("Threshold", &cfg::vizualizer::flStaticThreshold, 0.1, 1.f, "%.3f");
    }
    ImGui::EndChild();
}

void menu::Skins(ImVec2 savedCursorPosition) {

    using namespace cfg::skin;
    static const char* arrKnifeModels[] = { "Default", "Bayonet", "M9-Bayonet", "Karambit", "Bowie", "Butterfly", "Falchion", "Flip", "Gut", "Huntsman", "Shadow daggers", "Navaja", "Stiletto", "Talon", "Ursus", "Paracord", "Survival", "Nomad", "Skeleton", "Classic" };
    static const char* arrGloveModels[] = { "Default", "Bloodhound", "Sport", "Slick", "Handwraps", "Motorcycle", "Specialist", "Hydra", "Broken Fang" };

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().WindowPadding.x + 1);
    menu::DrawCustomChildRounding(("##TopBar"), ImVec2(ImGui::GetContentRegionAvail().x, 80), true, 0, ImDrawCornerFlags_TopRight);
    {
        ImGui::SameLine();
        ImGui::PushFont(tabIconsPreview);
        {
            ImGui::PushStyleColor(ImGuiCol_Text, clr::text_preview);
            menu::Text("e", 1.f, true);
            ImGui::PopStyleColor();
        }
        ImGui::PopFont();
    }
    ImGui::EndChild();

    ImVec2 Padding = ImVec2(ImGui::GetStyle().WindowPadding.x, ImGui::GetStyle().WindowPadding.y);
    ImGui::SetCursorPos(ImVec2(savedCursorPosition.x, savedCursorPosition.y + 80 + ImGui::GetStyle().WindowPadding.y));

    ImVec2 TopLeftSize = ImVec2(ImGui::GetContentRegionAvail().x / 2.f - Padding.x, ImGui::GetContentRegionAvail().y - Padding.y - 25.f);
    ImGui::BeginChild("##LeftWhole", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - Padding.x, ImGui::GetContentRegionAvail().y - Padding.y - 25.f), true);
    {
        static short iBackupItemDefinitionIndex = 0;
        static std::vector<std::string> vecSkinNames;
        static std::vector<int> vecSkinRarity;
        static std::vector<int> vecPaintKit;

        static std::string_view szSearchWord = "";
        static char buf[255]{};
        ImGui::InputText(("Search"), buf, sizeof(buf));

        szSearchWord = std::string_view(buf);

        static std::string_view szBackup = szSearchWord;
        if (iBackupItemDefinitionIndex != skinChanger.iItemDefinitionIndex || szBackup != szSearchWord || (bFilterByWeapon && vecSkinNames.size() >= skinChanger.SkinKits.size()) || (!bFilterByWeapon && vecSkinNames.size() < skinChanger.SkinKits.size())) {

            szBackup = szSearchWord;
            vecSkinNames.clear(); vecSkinRarity.clear(); vecPaintKit.clear();
            vecSkinNames.push_back("None");
            vecSkinRarity.push_back(1);
            vecPaintKit.push_back(0);
            for (SkinKit_t& it : skinChanger.SkinKits) {
                
                if (std::string_view cur = it.m_szName; szSearchWord != "") {

                    auto it = std::search(cur.begin(), cur.end(),
                        szSearchWord.begin(), szSearchWord.end(), [](char a, char b) {
                            return std::tolower(a) == std::tolower(b);
                        });

                    if (it == cur.end())
                        continue;
                }

                if (bFilterByWeapon) {

                    if (it.m_iWeaponID != skinChanger.iItemDefinitionIndex)
                        continue;
                }

                vecSkinNames.push_back(it.m_szName);
                vecSkinRarity.push_back(it.m_nRarity);
                vecPaintKit.push_back(it.m_nID);
            }
            iBackupItemDefinitionIndex = skinChanger.iItemDefinitionIndex;
        }
        int iMenuID = skinChanger.GetMenuFromId(skinChanger.iItemDefinitionIndex);

        if (!vecSkinNames.empty()) {

            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            static int iCurrentIndex[37]{ 0 };
            for (size_t i = 0; i < 37; i++) {
                auto it = std::find(vecPaintKit.begin(), vecPaintKit.end(), iSkinId[i]);
                if (it != vecPaintKit.end())
                    iCurrentIndex[i] = std::distance(vecPaintKit.begin(), it);
            }
            ImGui::ListBoxVectorSkin("##skins", &iCurrentIndex[iMenuID], vecSkinNames, vecSkinRarity, 20);
            if (ImGui::Button("Force Update", ImVec2(ImGui::GetContentRegionAvail().x, 25), true, true))
                skinChanger.bShouldFullUpdate = true;

            ImGui::PopItemWidth();

            if (vecPaintKit.size() > iCurrentIndex[iMenuID])
            {
                if (iSkinId[iMenuID] != vecPaintKit.at(iCurrentIndex[iMenuID]) && iMenuID != 35) {
                    skinChanger.bShouldFullUpdate = true;
                    iSkinId[iMenuID] = vecPaintKit.at(iCurrentIndex[iMenuID]);
                }
            }

			
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();
    ImVec2 NextWindowCursor = ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPosY() + ImGui::GetContentRegionAvail().y * 0.7f - 25.f);
    ImGui::BeginChild("##TopRight", ImVec2(ImGui::GetContentRegionAvail().x - Padding.x, ImGui::GetContentRegionAvail().y * 0.7f - Padding.y - 25.f), true);
    {
        int iMenuID = skinChanger.GetMenuFromId(skinChanger.iItemDefinitionIndex);

        ImGui::Checkbox("Enable##2", &bEnableSkinChanger);
        ImGui::Checkbox("Filter by weapon##2", &bFilterByWeapon);

        ImGui::SliderFloat("Wear", &flSkinWear[iMenuID], 0.f, 100.f, "%.4f");
        ImGui::SliderInt("Stattrak", &iSkinStattrak[iMenuID], 0, 2500);
        ImGui::SliderInt("Seed", &iSeed[iMenuID], 0, 1000);
        

        static char nameTagBuffer[37][255]{};
        ImGui::InputText(("Name tag"), nameTagBuffer[iMenuID], sizeof(nameTagBuffer));
        szSkinNametag[iMenuID] = nameTagBuffer[iMenuID];

        static const char* arrQualities[] = { "Normal", "Genuine", "Vintage", "Unusual", "Community",  "Developer", "Self-Made", "Customized", "Strange", "Completed", "Tournament" };
        static int iBackupQualities[37];
        for (size_t i = 0; i < 37; i++) 
            iBackupQualities[i] = iQuality[iMenuID];
        
        ImGui::ListBox("Quality", &iQuality[iMenuID], arrQualities, IM_ARRAYSIZE(arrQualities));
        if (iBackupQualities[iMenuID] != iQuality[iMenuID]) {
            iBackupQualities[iMenuID] = iQuality[iMenuID];
            skinChanger.bShouldFullUpdate = true;
        }

        if (ImGui::Checkbox("Skin color", &bModifySkinColors[iMenuID]))
            skinChanger.bShouldFullUpdate = true;

        if (bModifySkinColors[iMenuID]) {

            ImGui::Text("Color 1:");
            ImGui::ColorEdit4("##color1", colSkins1[iMenuID]);

            ImGui::Text("Color 2:");
            ImGui::ColorEdit4("##color2", colSkins2[iMenuID]);

            ImGui::Text("Color 3:");
            ImGui::ColorEdit4("##color3", colSkins3[iMenuID]);

            ImGui::Text("Color 4:");
            ImGui::ColorEdit4("##color4", colSkins4[iMenuID]);
        }
    }
    ImGui::EndChild();

    ImGui::SetCursorPos(NextWindowCursor);
    ImGui::BeginChild("##BotRight", ImVec2(ImGui::GetContentRegionAvail().x - Padding.x, ImGui::GetContentRegionAvail().y - Padding.y - 25.f), true);
    {
        int iMenuID = skinChanger.GetMenuFromId(skinChanger.iItemDefinitionIndex);

        int iSelectedKnifeModel = iKnifeModel;
        ImGui::Combo("Knife", &iKnifeModel, arrKnifeModels, IM_ARRAYSIZE(arrKnifeModels));
        skinChanger.bShouldFullUpdate = iSelectedKnifeModel != iKnifeModel ? true : skinChanger.bShouldFullUpdate;

        //Moved agent chabger to a diff tab so it will show even in spectator
        static std::vector<const char*> vecAgentList{};
        if (vecAgentList.empty())
            for (const AgentObject_t& it : skinChanger.agentList)
                vecAgentList.push_back(it.szDisplayName.c_str());

        ImGui::Combo("Agents", &iSkinId[36], vecAgentList.data(), vecAgentList.size());

        int iSelectedGloveModel = iGloveModel;
        ImGui::Combo("Glove", &iGloveModel, arrGloveModels, IM_ARRAYSIZE(arrGloveModels));
        skinChanger.bShouldFullUpdate = iSelectedGloveModel != iGloveModel ? true : skinChanger.bShouldFullUpdate;

        
        
        static std::vector<const char*> vecGloveNames[9];
        static  std::vector<int> vecPaintKit[9];

        static const char* arrGloveModelSearch[] = { "Default", "Bloodhound", "Sport", "Driver", "Wrap", "Moto", "Specialist", "Bloodhound", "Broken Fang" };
        static bool bOnce = true;
        if (bOnce)
            for (size_t j = 0; j < 9; j++)
            {
                std::string_view otherString = arrGloveModelSearch[j];
                for (SkinKit_t& it : skinChanger.GloveKits) {

                    std::string_view searchString = it.m_szName;
                    if (searchString.find(otherString) != std::string_view::npos) {

                        vecGloveNames[j].push_back(it.m_szName.c_str());
                        vecPaintKit[j].push_back(it.m_nID);
                    }
                }
            }
        bOnce = false;
        if (!vecGloveNames[iGloveModel].empty()) {

            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            static int iSelectedGlove[9];
            for (size_t i = 0; i < 9; i++) {
                auto it = std::find(vecPaintKit[i].begin(), vecPaintKit[i].end(), iSkinId[35]);
                if (it != vecPaintKit[i].end())
                    iSelectedGlove[i] = std::distance(vecPaintKit[i].begin(), it);
            }

            ImGui::Combo("##yes", &iSelectedGlove[iGloveModel], vecGloveNames[iGloveModel].data(), vecGloveNames[iGloveModel].size());
            skinChanger.bShouldFullUpdate = vecPaintKit[iGloveModel].at(iSelectedGlove[iGloveModel]) != iSkinId[35] ? true : skinChanger.bShouldFullUpdate;
            iSkinId[35] = vecPaintKit[iGloveModel].at(iSelectedGlove[iGloveModel]);

            ImGui::PopItemWidth();
        }


    }
    ImGui::EndChild();
}

void menu::Config(ImVec2 savedCursorPosition) {

    static int iSelect = 0;
    static std::string szSelectedScriptPath = "";
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().WindowPadding.x + 1);
    menu::DrawCustomChildRounding(("##TopBar"), ImVec2(ImGui::GetContentRegionAvail().x, 80), true, 0, ImDrawCornerFlags_TopRight);
    {
        ImGui::PushFont( defaultFontBigger );
        ImGui::SetCursorPosY( ImGui::GetCursorPosY( ) - ImGui::GetStyle( ).WindowPadding.y );

        ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, 0.f );

        ImVec2 size = ImVec2( ( ImGui::GetContentRegionAvail( ).x - 5 * 9 ) / 4.f, ImGui::GetContentRegionAvail( ).y + ImGui::GetStyle( ).WindowPadding.y );
        if (ImGui::Button( XorStr( "General" ), size, iSelect == 0 ))
            iSelect = 0;
        ImGui::SameLine( );

        if (ImGui::Button( XorStr( "Elements" ), size, iSelect == 1 ))
            iSelect = 1;

        ImGui::PopFont( );
        ImGui::SameLine();
        ImGui::PushFont(tabIconsPreview);
        {
            ImGui::PushStyleColor(ImGuiCol_Text, clr::text_preview);
            menu::Text("d", 1.f, true);
            ImGui::PopStyleColor();
        }
        ImGui::PopFont();
        ImGui::PopStyleVar( );
    }
    ImGui::EndChild();

    ImVec2 Padding = ImVec2(ImGui::GetStyle().WindowPadding.x, ImGui::GetStyle().WindowPadding.y);
    ImGui::SetCursorPos( ImVec2( savedCursorPosition.x, savedCursorPosition.y + 80 + ImGui::GetStyle( ).WindowPadding.y ) );
    ImGui::BeginChild(("##Left"), ImVec2(ImGui::GetContentRegionAvail().x / 2.f - Padding.x, ImGui::GetContentRegionAvail().y - Padding.y - 25.f), true);
    {
        if (iSelect == 0)
        {
            static std::string selectedConfig = "";
            ImGui::PushItemWidth( ImGui::GetContentRegionAvail( ).x );
            if (ImGui::ListBoxVector( ( "##configs" ), &cfg::configID, pConfig->vecConfigs, 15 )) {
                selectedConfig = pConfig->vecConfigs[ cfg::configID ];
            }

            static char buf[ 255 ]{};
            ImGui::InputText( ( "##Config name" ), buf, sizeof( buf ) );

            if (ImGui::Button( ( "Refresh" ), ImVec2( ImGui::GetContentRegionAvail( ).x, 20.f ), true, true )) {
                pConfig->RefreshConfigs( );
            }

            ImGui::Spacing( );
            if (ImGui::Button( ( "Save" ), ImVec2( ImGui::GetContentRegionAvail( ).x, 20.f ), true, true )) {
                bPressedSave = true;
                bWarningMethod = true;
            }

            ImGui::Spacing( );
            if (ImGui::Button( ( "Load" ), ImVec2( ImGui::GetContentRegionAvail( ).x, 20.f ), true, true )) {
                bPressedSave = true;
                bWarningMethod = false;
            }

            ImGui::Spacing( );
            if (ImGui::Button( ( "Create" ), ImVec2( ImGui::GetContentRegionAvail( ).x, 20.f ), true, true )) {

                pConfig->Save( buf );
                pConfig->RefreshConfigs( );
            }

            ImGui::Spacing( );
            if (ImGui::Button( ( "Delete" ), ImVec2( ImGui::GetContentRegionAvail( ).x, 20.f ), true, true )) {

                pConfig->DeleteConfig( selectedConfig );
                pConfig->RefreshConfigs( );
            }
            ImGui::Spacing( );
            if (ImGui::Button( ( "Reset" ), ImVec2( ImGui::GetContentRegionAvail( ).x, 20.f ), true, true )) {

                pConfig->bInitialized = false;
                pConfig->Setup( );
            }
            ImGui::Spacing( );
            if (ImGui::Button( ( "Open config location" ), ImVec2( ImGui::GetContentRegionAvail( ).x, 20.f ), true, true )) {

                ShellExecuteA( NULL, ( "open" ), pConfig->ConfigPath.c_str( ), NULL, NULL, SW_SHOWNORMAL );;
            }
            ImGui::Spacing( );
            if (ImGui::Button( ( "Unload" ), ImVec2( ImGui::GetContentRegionAvail( ).x, 20.f ), true, true )) {

                cfg::bDoUnload = true;
            }
            ImGui::PopItemWidth( );
        }
        else // scripts
        {
            
        }
    }
    ImGui::EndChild();

    ImGui::SameLine( );
    ImVec2 NextWindowCursor = ImVec2( ImGui::GetCursorPos( ).x, ImGui::GetCursorPosY( ) + ImGui::GetContentRegionAvail( ).y * 0.7f - 25.f );
    ImGui::BeginChild( XorStr( "##Right" ), ImVec2( ImGui::GetContentRegionAvail( ).x - Padding.x, ImGui::GetContentRegionAvail( ).y - Padding.y - 25.f ), true );
    {
        if (ImGui::BeginListBox( XorStr( "##scripts" ), ImVec2( -1, ImGui::GetContentRegionAvail( ).y - ( ( ImGui::GetFrameHeight( ) + ImGui::GetStyle( ).ItemSpacing.y ) * 5 ) - ( ImGui::GetStyle( ).FramePadding.y * 2 ) - 5.0f ) ))
        {
            
            ImGui::EndListBox( );
        }

        if (ImGui::Button( XorStr( "Refresh" ), ImVec2( ImGui::GetContentRegionAvail( ).x, 20.f ), true, true )) {
           
        }


        if (ImGui::Button( XorStr( "Open script location" ), ImVec2( ImGui::GetContentRegionAvail( ).x, 20.f ), true, true ))
        {

        }
      
    }
    ImGui::EndChild( );
}

void menu::PlayerList(ImVec2 savedCursorPosition) {

	static int iSelectedEntity = 0;
	std::vector<playerSettings_t> vecPlayers = playerList::GetPlayers();

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().WindowPadding.x + 1);
    menu::DrawCustomChildRounding(("##TopBar"), ImVec2(ImGui::GetContentRegionAvail().x, 80), true, 0, ImDrawCornerFlags_TopRight);
    {
        ImGui::SameLine();
        ImGui::PushFont(tabIconsPreview);
        {
            ImGui::PushStyleColor(ImGuiCol_Text, clr::text_preview);
            menu::Text("b", 1.f, true);
            ImGui::PopStyleColor();
        }
        ImGui::PopFont();
    }
    ImGui::EndChild();

    ImVec2 Padding = ImVec2(ImGui::GetStyle().WindowPadding.x, ImGui::GetStyle().WindowPadding.y);
    ImGui::SetCursorPos(ImVec2(savedCursorPosition.x, savedCursorPosition.y + 80 + ImGui::GetStyle().WindowPadding.y));

    ImVec2 TopLeftSize = ImVec2(ImGui::GetContentRegionAvail().x / 2.f - Padding.x, ImGui::GetContentRegionAvail().y - Padding.y - 25.f);
    ImGui::BeginChild("##LeftWhole", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - Padding.x, ImGui::GetContentRegionAvail().y - Padding.y - 25.f), true);
    {
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		{
			std::vector<std::string> vecNames = playerList::GetNames(vecPlayers);
			ImGui::ListBoxVector("##players", &iSelectedEntity, vecNames, 15);
		}
		ImGui::PopItemWidth();

		if (vecPlayers.size() <= iSelectedEntity)
			return ImGui::EndChild();

		playerSettings_t& refCurrentSelected = playerList::arrPlayers[vecPlayers.at(iSelectedEntity).iIndex];

		ImGui::Checkbox("Prioritize", &refCurrentSelected.bPriority);
		ImGui::Checkbox("WhiteList", &refCurrentSelected.bWhiteList);
    }
    ImGui::EndChild();

	ImGui::SetCursorPos(ImVec2(savedCursorPosition.x + TopLeftSize.x + Padding.x, savedCursorPosition.y + 80 + ImGui::GetStyle().WindowPadding.y));
	ImGui::BeginChild(("##RightWhole"), ImVec2(ImGui::GetContentRegionAvail().x - Padding.x, ImGui::GetContentRegionAvail().y - Padding.y - 25.f), true);
	{
		if (vecPlayers.size() <= iSelectedEntity)
			return ImGui::EndChild();

		playerSettings_t& refCurrentSelected = playerList::arrPlayers[vecPlayers.at(iSelectedEntity).iIndex];

		if (ImGui::Button("Force safe", ImVec2(100, 20), refCurrentSelected.bSafePoint, true))
			refCurrentSelected.bSafePoint = !refCurrentSelected.bSafePoint;

		ImGui::SameLine();
		if (ImGui::Button("Override yaw", ImVec2(100, 20), refCurrentSelected.bOverrideResolver, true))
			refCurrentSelected.bOverrideResolver = !refCurrentSelected.bOverrideResolver;

		if (refCurrentSelected.bOverrideResolver)
			ImGui::SliderInt("Override", &refCurrentSelected.flOverrideYaw, -58, 58);

		if (refCurrentSelected.bLocalPlayer) {
			if (ImGui::Button("Reset name", ImVec2(100, 20), true, true))
				misc::ResetName(false, std::format("{}\n", refCurrentSelected.playerInfo.szName).c_str());
		}
		else
			if (ImGui::Button("Steal name", ImVec2(100, 20), true, true))
				misc::ChangeName(false, std::format("{}\n", refCurrentSelected.playerInfo.szName).c_str());

		ImGui::SameLine();
		if (ImGui::Button("Vote kick", ImVec2(100, 20), true, true))
			i::EngineClient->ExecuteClientCmd(std::format("callvote kick {} {}\n", refCurrentSelected.playerInfo.nUserID, refCurrentSelected.iIndex).c_str());

		if (ImGui::Button("Copy name", ImVec2(100, 20), true, true))
			util::CopyToClipboard(refCurrentSelected.playerInfo.szName);

		ImGui::SameLine();
		if (ImGui::Button("Copy steamID", ImVec2(100, 20), true, true))
			util::CopyToClipboard(refCurrentSelected.playerInfo.szSteamID);
	}
	ImGui::EndChild();
}

void menu::SaveWarning(bool& saved, bool type) noexcept {

    ImGui::SetNextWindowSizeConstraints(ImVec2(180, 101), ImVec2(180, 101));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(180, 101)); 
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.f);

    ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x * 0.5f) - (180 * 0.5f), ImGui::GetIO().DisplaySize.y * 0.5f - (101 * 0.5f)));
    ImGui::Begin(("##savestuff"), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    {
        ImGui::BeginChild(("##yes"), ImGui::GetContentRegionAvail(), true);
        {
            ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 4 - 3, ImGui::GetContentRegionAvail().y / 5));
            ImGui::Text(("Are you sure?"));

            static ImVec2 buttonSize = ImVec2(ImGui::GetContentRegionAvail().x / 3, ImGui::GetContentRegionAvail().y / 2);
            if (ImGui::Button(("Yes"), buttonSize, true, false)) {
                saved = false;

                if (type)
                {
                    // reset
                    cfg::vecScripts.clear( );
                    cfg::vecScriptVariables.clear( );

                   
                    pConfig->Save(pConfig->vecConfigs[ cfg::configID ] );
                }
                else
                {
                    pConfig->Load(pConfig->vecConfigs[ cfg::configID ] );
                }
            }
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x / 2);
            if (ImGui::Button(("No"), buttonSize, true, false))
                saved = false;
        }
        ImGui::EndChild();
    }
    ImGui::PopStyleVar(2); 
    ImGui::End();
}