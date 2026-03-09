#pragma once

#include <imgui.h>
#include <unordered_map>
#include <string>
#include <mq/Plugin.h>

class Ui
{
public:

	struct TooltipState
	{
		int was_hovered;
		float tooltip_time;
	};

	struct TrendState
	{
		float lastPct;
		int direction;
	};

	struct AnimState
	{
		float lastTarget;
	};

	struct SettingsStruct
	{
		ImVec2 Padding = ImVec2(8, 4);
		ImVec2 CursorPos = ImVec2(0, 0);
		ImVec2 LastCursorLinePos = ImVec2(0, 0);

		int FontSize = 20;
		float IconSize = 20.0f;
		float LineStartXPos = 0.0f;

		float BarRounding = 6.0f;
		float BarBorderThickness = 2.5f;

		std::unordered_map<std::string, TrendState> ProgBarTrendState;
		std::unordered_map<std::string, AnimState> ProgBarAnimState;

		TooltipState TooltipAnimationState;
	};

	static void SetCursorPos(const ImVec2& pos);
	static ImVec2 GetCursorPos();

	static void MoveCursor(const ImVec2& pos);
	static void SameLine();

	static float GetDeltaTime();

	static ImU32 ImVec4ToColor(const ImVec4& v);

	static ImVec4 GetConColor(const int color);
	static ImVec4 GetConColorBySpawn(SPAWNINFO* pSpawn);

	static void RenderNamePlateText(ImU32 color, const char* text);

	static void RenderNamePlateRect(
		const ImVec2& size,
		ImU32 color,
		float rounding,
		float thickness,
		bool filled
	);

	static void DrawInspectableSpellIcon(
		int iconID,
		EQ_Spell* pSpell
	);

	static void RenderAnimatedPercentage(
		const std::string& id,
		float barPct,
		float height,
		float width,
		const ImVec4& colLow,
		const ImVec4& colMid,
		const ImVec4& colHigh,
		ImU32 colHighlight,
		const std::string& label = ""
	);

	static void RenderFancyHPBar(
		const std::string& id,
		float hpPct,
		float height,
		float width,
		ImU32 hpHighlight,
		const std::string& label = ""
	);

	static SettingsStruct Settings;
};