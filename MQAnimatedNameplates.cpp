// MQAnimatedNameplates.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup.

// By: Derple derple@ntsj.com

#include <mq/Plugin.h>
#include <eqlib/graphics/CameraInterface.h>
#include <eqlib/game/Display.h>
#include "imgui/imgui.h"
#include "mq/imgui/ImGuiUtils.h"
#include <sol/sol.hpp>
#include "Ui.h"
#include "main\MQ2Inlines.h"

PreSetup("MQAnimatedNameplates");
PLUGIN_VERSION(0.1);

/**
 * Avoid Globals if at all possible, since they persist throughout your program.
 * But if you must have them, here is the place to put them.
 */
// bool ShowMQAnimatedNameplatesWindow = true;

/**
 * @fn InitializePlugin
 *
 * This is called once on plugin initialization and can be considered the startup
 * routine for the plugin.
 */
PLUGIN_API void InitializePlugin()
{
	DebugSpewAlways("MQAnimatedNameplates::Initializing version %f", MQ2Version);

	// Examples:
	// AddCommand("/mycommand", MyCommand);
	// AddXMLFile("MQUI_MyXMLFile.xml");
	// AddMQ2Data("mytlo", MyTLOData);
}

/**
 * @fn ShutdownPlugin
 *
 * This is called once when the plugin has been asked to shutdown.  The plugin has
 * not actually shut down until this completes.
 */
PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("MQAnimatedNameplates::Shutting down");

	// Examples:
	// RemoveCommand("/mycommand");
	// RemoveXMLFile("MQUI_MyXMLFile.xml");
	// RemoveMQ2Data("mytlo");
}

/**
 * @fn OnCleanUI
 *
 * This is called once just before the shutdown of the UI system and each time the
 * game requests that the UI be cleaned.  Most commonly this happens when a
 * /loadskin command is issued, but it also occurs when reaching the character
 * select screen and when first entering the game.
 *
 * One purpose of this function is to allow you to destroy any custom windows that
 * you have created and cleanup any UI items that need to be removed.
 */
PLUGIN_API void OnCleanUI()
{
	// DebugSpewAlways("MQAnimatedNameplates::OnCleanUI()");
}

/**
 * @fn OnReloadUI
 *
 * This is called once just after the UI system is loaded. Most commonly this
 * happens when a /loadskin command is issued, but it also occurs when first
 * entering the game.
 *
 * One purpose of this function is to allow you to recreate any custom windows
 * that you have setup.
 */
PLUGIN_API void OnReloadUI()
{
	// DebugSpewAlways("MQAnimatedNameplates::OnReloadUI()");
}

/**
 * @fn OnDrawHUD
 *
 * This is called each time the Heads Up Display (HUD) is drawn.  The HUD is
 * responsible for the net status and packet loss bar.
 *
 * Note that this is not called at all if the HUD is not shown (default F11 to
 * toggle).
 *
 * Because the net status is updated frequently, it is recommended to have a
 * timer or counter at the start of this call to limit the amount of times the
 * code in this section is executed.
 */
PLUGIN_API void OnDrawHUD()
{
/*
	static std::chrono::steady_clock::time_point DrawHUDTimer = std::chrono::steady_clock::now();
	// Run only after timer is up
	if (std::chrono::steady_clock::now() > DrawHUDTimer)
	{
		// Wait half a second before running again
		DrawHUDTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
		DebugSpewAlways("MQAnimatedNameplates::OnDrawHUD()");
	}
*/
}

/**
 * @fn SetGameState
 *
 * This is called when the GameState changes.  It is also called once after the
 * plugin is initialized.
 *
 * For a list of known GameState values, see the constants that begin with
 * GAMESTATE_.  The most commonly used of these is GAMESTATE_INGAME.
 *
 * When zoning, this is called once after @ref OnBeginZone @ref OnRemoveSpawn
 * and @ref OnRemoveGroundItem are all done and then called once again after
 * @ref OnEndZone and @ref OnAddSpawn are done but prior to @ref OnAddGroundItem
 * and @ref OnZoned
 *
 * @param GameState int - The value of GameState at the time of the call
 */
PLUGIN_API void SetGameState(int GameState)
{
	// DebugSpewAlways("MQAnimatedNameplates::SetGameState(%d)", GameState);
}


/**
 * @fn OnPulse
 *
 * This is called each time MQ2 goes through its heartbeat (pulse) function.
 *
 * Because this happens very frequently, it is recommended to have a timer or
 * counter at the start of this call to limit the amount of times the code in
 * this section is executed.
 */
PLUGIN_API void OnPulse()
{
}

/**
 * @fn OnWriteChatColor
 *
 * This is called each time WriteChatColor is called (whether by MQ2Main or by any
 * plugin).  This can be considered the "when outputting text from MQ" callback.
 *
 * This ignores filters on display, so if they are needed either implement them in
 * this section or see @ref OnIncomingChat where filters are already handled.
 *
 * If CEverQuest::dsp_chat is not called, and events are required, they'll need to
 * be implemented here as well.  Otherwise, see @ref OnIncomingChat where that is
 * already handled.
 *
 * For a list of Color values, see the constants for USERCOLOR_.  The default is
 * USERCOLOR_DEFAULT.
 *
 * @param Line const char* - The line that was passed to WriteChatColor
 * @param Color int - The type of chat text this is to be sent as
 * @param Filter int - (default 0)
 */
PLUGIN_API void OnWriteChatColor(const char* Line, int Color, int Filter)
{
	// DebugSpewAlways("MQAnimatedNameplates::OnWriteChatColor(%s, %d, %d)", Line, Color, Filter);
}

/**
 * @fn OnIncomingChat
 *
 * This is called each time a line of chat is shown.  It occurs after MQ filters
 * and chat events have been handled.  If you need to know when MQ2 has sent chat,
 * consider using @ref OnWriteChatColor instead.
 *
 * For a list of Color values, see the constants for USERCOLOR_. The default is
 * USERCOLOR_DEFAULT.
 *
 * @param Line const char* - The line of text that was shown
 * @param Color int - The type of chat text this was sent as
 *
 * @return bool - Whether to filter this chat from display
 */
PLUGIN_API bool OnIncomingChat(const char* Line, DWORD Color)
{
	// DebugSpewAlways("MQAnimatedNameplates::OnIncomingChat(%s, %d)", Line, Color);
	return false;
}

/**
 * @fn OnAddSpawn
 *
 * This is called each time a spawn is added to a zone (ie, something spawns). It is
 * also called for each existing spawn when a plugin first initializes.
 *
 * When zoning, this is called for all spawns in the zone after @ref OnEndZone is
 * called and before @ref OnZoned is called.
 *
 * @param pNewSpawn PSPAWNINFO - The spawn that was added
 */
PLUGIN_API void OnAddSpawn(PSPAWNINFO pNewSpawn)
{
	// DebugSpewAlways("MQAnimatedNameplates::OnAddSpawn(%s)", pNewSpawn->Name);
}

/**
 * @fn OnRemoveSpawn
 *
 * This is called each time a spawn is removed from a zone (ie, something despawns
 * or is killed).  It is NOT called when a plugin shuts down.
 *
 * When zoning, this is called for all spawns in the zone after @ref OnBeginZone is
 * called.
 *
 * @param pSpawn PSPAWNINFO - The spawn that was removed
 */
PLUGIN_API void OnRemoveSpawn(PSPAWNINFO pSpawn)
{
	// DebugSpewAlways("MQAnimatedNameplates::OnRemoveSpawn(%s)", pSpawn->Name);
}

/**
 * @fn OnAddGroundItem
 *
 * This is called each time a ground item is added to a zone (ie, something spawns).
 * It is also called for each existing ground item when a plugin first initializes.
 *
 * When zoning, this is called for all ground items in the zone after @ref OnEndZone
 * is called and before @ref OnZoned is called.
 *
 * @param pNewGroundItem PGROUNDITEM - The ground item that was added
 */
PLUGIN_API void OnAddGroundItem(PGROUNDITEM pNewGroundItem)
{
	// DebugSpewAlways("MQAnimatedNameplates::OnAddGroundItem(%d)", pNewGroundItem->DropID);
}

/**
 * @fn OnRemoveGroundItem
 *
 * This is called each time a ground item is removed from a zone (ie, something
 * despawns or is picked up).  It is NOT called when a plugin shuts down.
 *
 * When zoning, this is called for all ground items in the zone after
 * @ref OnBeginZone is called.
 *
 * @param pGroundItem PGROUNDITEM - The ground item that was removed
 */
PLUGIN_API void OnRemoveGroundItem(PGROUNDITEM pGroundItem)
{
	// DebugSpewAlways("MQAnimatedNameplates::OnRemoveGroundItem(%d)", pGroundItem->DropID);
}

/**
 * @fn OnBeginZone
 *
 * This is called just after entering a zone line and as the loading screen appears.
 */
PLUGIN_API void OnBeginZone()
{
	// DebugSpewAlways("MQAnimatedNameplates::OnBeginZone()");
}

/**
 * @fn OnEndZone
 *
 * This is called just after the loading screen, but prior to the zone being fully
 * loaded.
 *
 * This should occur before @ref OnAddSpawn and @ref OnAddGroundItem are called. It
 * always occurs before @ref OnZoned is called.
 */
PLUGIN_API void OnEndZone()
{
	// DebugSpewAlways("MQAnimatedNameplates::OnEndZone()");
}

/**
 * @fn OnZoned
 *
 * This is called after entering a new zone and the zone is considered "loaded."
 *
 * It occurs after @ref OnEndZone @ref OnAddSpawn and @ref OnAddGroundItem have
 * been called.
 */
PLUGIN_API void OnZoned()
{
	// DebugSpewAlways("MQAnimatedNameplates::OnZoned()");
}

/**
 * @fn OnUpdateImGui
 *
 * This is called each time that the ImGui Overlay is rendered. Use this to render
 * and update plugin specific widgets.
 *
 * Because this happens extremely frequently, it is recommended to move any actual
 * work to a separate call and use this only for updating the display.
 */

PLUGIN_API void OnUpdateImGui()
{
	if (GetGameState() == GAMESTATE_INGAME)
	{
		if (!pTarget || !pDisplay)
			return;

		const CVector3 targetPos(pTarget->Y , pTarget->X , pTarget->Z + pTarget->Height);
		float targetNameplatePosX, targetNameplatePosY;

		pDisplay->pCamera->ProjectWorldCoordinatesToScreen(targetPos, targetNameplatePosX, targetNameplatePosY);
		bool open = true;

		auto drawList = ImGui::GetForegroundDrawList();
		ImGui::PushFont(NULL, Ui::Settings.FontSize);

		const char* targetName = pTarget->DisplayedName;
		int pctHP = pTarget->HPMax == 0 ? 0 : pTarget->HPCurrent * 100 / pTarget->HPMax;
		char targetPctHPs[16];
		sprintf(targetPctHPs, "%d%%", pctHP);

		char classInfo[64];
		sprintf(classInfo, "%d %s",
			static_cast<int>(pTarget->GetLevel()),
			GetClassDesc(pTarget->GetClass()));

		ImVec2 canvasSize(500, 50);

		int buffCount = GetCachedBuffCount(pTarget);

		ImVec2 assumedHeadOffset(
			0,
			35 + (ceil(buffCount / 10.0f) * (Ui::Settings.IconSize + Ui::Settings.Padding.y))
		);

		ImVec2 curPos(
			targetNameplatePosX - canvasSize.x * 0.5f - assumedHeadOffset.x,
			targetNameplatePosY - canvasSize.y * 0.5f - assumedHeadOffset.y
		);

		Ui::SetCursorPos(ImVec2(
			curPos.x + Ui::Settings.Padding.x,
			curPos.y + Ui::Settings.Padding.y
		));
		int buffsPerRow = std::floor(canvasSize.x / (Ui::Settings.IconSize + Ui::Settings.Padding.x));

		for (int i = 0; i < buffCount; i++)
		{
			auto buff = GetCachedBuffAtSlot(pTarget, i);

			if (buff.has_value())
			{
				EQ_Spell* spell = GetSpellByID(buff->spellId);
				if (spell)
				{
					Ui::DrawInspectableSpellIcon(spell->SpellIcon, spell);

					if (i == 0 || (i < (buffCount-1) && i % buffsPerRow != 0))
						Ui::SameLine();
				}
			}
		}

		ImVec2 panelPos = Ui::GetCursorPos();
		
		Ui::RenderNamePlateRect(
			canvasSize,
			Ui::ImVec4ToColor(ImVec4(0.6f, 0.6f, 0.6f, 0.8f)),
			2,
			0,
			true
		);

		panelPos.x += Ui::Settings.Padding.x;

		Ui::SetCursorPos(panelPos);

		ImU32 textColor = IM_COL32(255, 255, 255, 255);

		auto con = Ui::GetConColorBySpawn(pTarget);
		ImU32 conColor = IM_COL32(
			(int)(con.w * 255),
			(int)(con.x * 255),
			(int)(con.y * 255),
			(int)(con.z * 255)
		);

		curPos = Ui::GetCursorPos();
		float startXPos = curPos.x;

		Ui::RenderNamePlateText(textColor, targetName);
		
		// center this text
		float classWidth = ImGui::CalcTextSize(classInfo).x;
		curPos.x = (curPos.x + canvasSize.x / 2) -
			(classWidth / 2 + Ui::Settings.Padding.x * 2);

		Ui::SetCursorPos(curPos);
		Ui::RenderNamePlateText(textColor, classInfo);

		// right justify this text
		float hpWidth = ImGui::CalcTextSize(targetPctHPs).x;
		curPos.x = (startXPos + canvasSize.x) -
			(hpWidth + Ui::Settings.Padding.x * 2);

		Ui::SetCursorPos(curPos);
		Ui::RenderNamePlateText(textColor, targetPctHPs);

		Ui::SetCursorPos(ImVec2(startXPos, Ui::GetCursorPos().y));

		std::string hpBarID =
			std::string("TargetHPBar_") +
			std::to_string(pTarget->GetId());

		Ui::RenderFancyHPBar(
			hpBarID.c_str(),
			pctHP,
			ImGui::GetTextLineHeight() * 0.75f,
			canvasSize.x - Ui::Settings.Padding.x * 2,
			conColor,
			""
		);
		ImGui::PopFont();
	}
}

/**
 * @fn OnMacroStart
 *
 * This is called each time a macro starts (ex: /mac somemacro.mac), prior to
 * launching the macro.
 *
 * @param Name const char* - The name of the macro that was launched
 */
PLUGIN_API void OnMacroStart(const char* Name)
{
	// DebugSpewAlways("MQAnimatedNameplates::OnMacroStart(%s)", Name);
}

/**
 * @fn OnMacroStop
 *
 * This is called each time a macro stops (ex: /endmac), after the macro has ended.
 *
 * @param Name const char* - The name of the macro that was stopped.
 */
PLUGIN_API void OnMacroStop(const char* Name)
{
	// DebugSpewAlways("MQAnimatedNameplates::OnMacroStop(%s)", Name);
}

/**
 * @fn OnLoadPlugin
 *
 * This is called each time a plugin is loaded (ex: /plugin someplugin), after the
 * plugin has been loaded and any associated -AutoExec.cfg file has been launched.
 * This means it will be executed after the plugin's @ref InitializePlugin callback.
 *
 * This is also called when THIS plugin is loaded, but initialization tasks should
 * still be done in @ref InitializePlugin.
 *
 * @param Name const char* - The name of the plugin that was loaded
 */
PLUGIN_API void OnLoadPlugin(const char* Name)
{
	// DebugSpewAlways("MQAnimatedNameplates::OnLoadPlugin(%s)", Name);
	/*
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigViewportsNoTaskBarIcon = false;
	io.ConfigViewportsNoDefaultParent = true;
	io.ConfigViewportsNoAutoMerge = false;
	io.ConfigViewportsNoDecoration = true;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;

	// setup fonts
	io.Fonts->Clear();
	mq::imgui::ConfigureFonts(io.Fonts);
	IM_ASSERT(io.Fonts->Build() && "ImGui::GetIO().Fonts->Build() failed!");

	mq::imgui::ConfigureStyle();
	*/
	}

/**
 * @fn OnUnloadPlugin
 *
 * This is called each time a plugin is unloaded (ex: /plugin someplugin unload),
 * just prior to the plugin unloading.  This means it will be executed prior to that
 * plugin's @ref ShutdownPlugin callback.
 *
 * This is also called when THIS plugin is unloaded, but shutdown tasks should still
 * be done in @ref ShutdownPlugin.
 *
 * @param Name const char* - The name of the plugin that is to be unloaded
 */
PLUGIN_API void OnUnloadPlugin(const char* Name)
{
	// DebugSpewAlways("MQAnimatedNameplates::OnUnloadPlugin(%s)", Name);
	//ImGui::DestroyContext();
}

sol::object DoCreateModule(sol::this_state s)
{
	sol::state_view L(s);

	sol::table module = L.create_table();

	module["ProjectWorldCoordinatesToScreen"] = [](sol::this_state L, const float x, const float y, const float z) -> ImVec2
		{

			if (!pDisplay || !pDisplay->pCamera)
				return ImVec2(0, 0);

			float outX, outY;

			pDisplay->pCamera->ProjectWorldCoordinatesToScreen(CVector3(y, x, z), outX, outY);

			return ImVec2(outX, outY);
		};

	return sol::make_object(L, module);
}

PLUGIN_API sol::object CreateLuaModule(sol::this_state s)
{
	return DoCreateModule(s);
}