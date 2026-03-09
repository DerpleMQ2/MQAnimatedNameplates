#include "Ui.h"
#include <algorithm>
#include <cmath>
#include "mq\imgui\Widgets.h"

Ui::SettingsStruct Ui::Settings;

void Ui::SetCursorPos(const ImVec2& pos)
{
	Settings.CursorPos = pos;
	Settings.LastCursorLinePos = pos;
	Settings.LineStartXPos = pos.x;
}

ImVec2 Ui::GetCursorPos()
{
	return Settings.CursorPos;
}

void Ui::MoveCursor(const ImVec2& pos)
{
	ImVec2 p(
		pos.x + Settings.Padding.x,
		pos.y + Settings.Padding.y
	);

	Settings.LastCursorLinePos.x = Settings.CursorPos.x + p.x;
	Settings.LastCursorLinePos.y = Settings.CursorPos.y;

	Settings.CursorPos.y += p.y;
	Settings.CursorPos.x = Settings.LineStartXPos;
}

void Ui::SameLine()
{
	Settings.CursorPos = Settings.LastCursorLinePos;
}

float Ui::GetDeltaTime()
{
	float dt = ImGui::GetIO().DeltaTime;

	if (dt <= 0.0f)
		dt = 1.0f / 60.0f;

	if (dt > 0.1f)
		dt = 0.1f;

	return dt;
}

ImU32 Ui::ImVec4ToColor(const ImVec4& v)
{
	return IM_COL32(
		(int)(v.x * 255.0f),
		(int)(v.y * 255.0f),
		(int)(v.z * 255.0f),
		(int)(v.w * 255.0f)
	);
}

ImVec4 Ui::GetConColor(const int color)
{
	switch (color)
	{
	case CONCOLOR_GREY:
		return { 0.6f, 0.6f, 0.6f, 0.8f };
	case CONCOLOR_GREEN:
		return { 0.02f, 0.8f, 0.2f, 0.8f };
	case CONCOLOR_LIGHTBLUE:
		return { 0.02f, 0.8f, 1.0f, 0.8f };
	case CONCOLOR_BLUE:
		return { 0.02f, 0.4f, 1.0f, 1.0f };
	case CONCOLOR_YELLOW:
		return { 0.8f, 0.8f, 0.02f, 0.8f };
	case CONCOLOR_RED:
		return { 0.8f, 0.2f, 0.2f, 0.8f };
	default:
		return { 1, 1, 1, 1 };
	}
}

ImVec4 Ui::GetConColorBySpawn(SPAWNINFO * pSpawn)
{
	return Ui::GetConColor(ConColor(pSpawn));
}

void Ui::RenderNamePlateText(ImU32 color, const char* text)
{
	ImVec2 size = ImGui::CalcTextSize(text);

	ImDrawList* dl = ImGui::GetForegroundDrawList();
	dl->AddText(Settings.CursorPos, color, text);

	MoveCursor(size);
}

void Ui::RenderNamePlateRect(
	const ImVec2& size,
	ImU32 color,
	float rounding,
	float thickness,
	bool filled)
{
	ImDrawList* dl = ImGui::GetForegroundDrawList();

	ImVec2 max(
		Settings.CursorPos.x + size.x,
		Settings.CursorPos.y + size.y
	);

	if (filled)
	{
		dl->AddRectFilled(Settings.CursorPos, max, color, rounding);
	}
	else
	{
		dl->AddRect(Settings.CursorPos, max, color, rounding, 0, thickness);
	}

	MoveCursor(size);
}

void Ui::DrawInspectableSpellIcon(int iconID, EQ_Spell* pSpell)
{
	ImVec2 cursor = GetCursorPos();

	ImVec2 size(Settings.IconSize, Settings.IconSize);
	ImVec2 max(cursor.x + size.x, cursor.y + size.y);

	ImDrawList* dl = ImGui::GetForegroundDrawList();

	ImVec2 mouse = ImGui::GetIO().MousePos;

	bool hovered =
		mouse.x >= cursor.x && mouse.x <= max.x &&
		mouse.y >= cursor.y && mouse.y <= max.y;

	bool clicked = hovered && ImGui::IsMouseClicked(0);

	if (pSidlMgr)
	{
		if (CTextureAnimation* anim = pSidlMgr->FindAnimation(CXStr("A_SpellGems")))
		{
			if (anim)
			{
				anim->SetCurCell(iconID);
				imgui::DrawTextureAnimation(dl, anim, cursor, size);
			}
		}
	}

	if (clicked && pSpell)
	{
		if (pSpellDisplayManager)
			pSpellDisplayManager->ShowSpell(pSpell->ID, true, true, SpellDisplayType_SpellBookWnd);
	}

	MoveCursor(size);
}

// helper
static float Clamp(float v, float min, float max)
{
	return std::max(min, std::min(max, v));
}

static ImVec4 ImLerp(const ImVec4& a, const ImVec4& b, float t)
{
	return ImVec4(
		a.x + (b.x - a.x) * t,
		a.y + (b.y - a.y) * t,
		a.z + (b.z - a.z) * t,
		a.w + (b.w - a.w) * t
	);
}

void Ui::RenderAnimatedPercentage(
	const std::string& id,
	float barPct,
	float height,
	float width,
	const ImVec4& colLow,
	const ImVec4& colMid,
	const ImVec4& colHigh,
	ImU32 colHighlight,
	const std::string& label)
{
	float targetPct = Clamp(barPct, 0.0f, 100.0f);

	double now = ImGui::GetTime();
	ImDrawList* drawList = ImGui::GetForegroundDrawList();

	float pct = targetPct;

	AnimState& animState = Settings.ProgBarAnimState[id];

	if (animState.lastTarget == 0)
		animState.lastTarget = targetPct;

	if (targetPct != animState.lastTarget)
		animState.lastTarget = targetPct;

	// simple animation (instead of ImAnim tween)
	float dt = GetDeltaTime();
	pct = animState.lastTarget + (targetPct - animState.lastTarget) * (dt * 8.0f);
	animState.lastTarget = pct;

	float fraction = pct / 100.0f;

	TrendState& trend = Settings.ProgBarTrendState[id];

	if (trend.lastPct == 0)
	{
		trend.lastPct = pct;
		trend.direction = 1;
	}
	else
	{
		if (targetPct < (trend.lastPct - 0.05f))
			trend.direction = -1;
		else if (targetPct > (trend.lastPct + 0.05f))
			trend.direction = 1;

		trend.lastPct = pct;
	}

	ImVec2 curPos = GetCursorPos();

	float minX = curPos.x;
	float minY = curPos.y;
	float maxX = curPos.x + width;
	float maxY = curPos.y + height;

	float barW = width;
	float barH = height;

	ImU32 bgTop = IM_COL32(28, 30, 41, 247);
	ImU32 bgBottom = IM_COL32(10, 13, 20, 247);

	drawList->AddRectFilledMultiColor(
		ImVec2(minX + 1, minY + 1),
		ImVec2(maxX - 1, maxY - 1),
		bgTop, bgTop, bgBottom, bgBottom
	);

	drawList->AddRectFilled(
		ImVec2(minX + 1, minY + 1),
		ImVec2(maxX - 1, minY + std::max(2.0f, barH * 0.35f)),
		IM_COL32(255, 255, 255, 14),
		Settings.BarRounding
	);

	float fillWidth = barW * fraction;

	if (fillWidth > 0)
	{
		ImVec4 edge;

		if (fraction < 0.5f)
		{
			float t = fraction / 0.5f;
			edge = ImLerp(colLow, colMid, t);
		}
		else
		{
			float t = (fraction - 0.5f) / 0.5f;
			edge = ImLerp(colMid, colHigh, t);
		}

		ImU32 topLeft = ImGui::GetColorU32(colLow);
		ImU32 topRight = ImGui::GetColorU32(edge);
		ImU32 bottomLeft = topLeft;
		ImU32 bottomRight = topRight;

		float fillMaxX = minX + fillWidth;

		float fillRounding = std::min(
			Settings.BarRounding,
			std::min(barH * 0.5f, fillWidth * 0.5f)
		);

		drawList->AddRectFilled(
			ImVec2(minX, minY),
			ImVec2(fillMaxX, maxY),
			ImVec4ToColor(colLow),
			fillRounding
		);

		float innerMinX = minX + 1;
		float innerMaxX = fillMaxX - 1;
		float innerMinY = minY + 1;
		float innerMaxY = maxY - 1;

		if (innerMaxX > innerMinX && innerMaxY > innerMinY)
		{
			drawList->AddRectFilledMultiColor(
				ImVec2(innerMinX, innerMinY),
				ImVec2(innerMaxX, innerMaxY),
				topLeft, topRight, bottomRight, bottomLeft
			);

			float glossMaxY = std::min(innerMaxY, minY + std::max(2.0f, barH * 0.45f));

			if (glossMaxY > innerMinY)
			{
				drawList->AddRectFilledMultiColor(
					ImVec2(innerMinX, innerMinY),
					ImVec2(innerMaxX, glossMaxY),
					IM_COL32(255, 255, 255, 14),
					IM_COL32(255, 255, 255, 8),
					IM_COL32(255, 255, 255, 2),
					IM_COL32(255, 255, 255, 8)
				);
			}
		}

		if (fillWidth > 12)
		{
			bool isAnimating = fabs(targetPct - pct) > 0.5f;

			float sweepSpeed = isAnimating ? 1.2f : 0.65f;
			float sweepBase = fmod(now * sweepSpeed, 1.0f);

			float sweep = (isAnimating || trend.direction < 0)
				? (1.0f - sweepBase)
				: sweepBase;

			float sheenCenter = minX + (fillWidth * sweep);
			float sheenHalf = std::min(16.0f, fillWidth * 0.22f);

			float sheenLeft = std::max(minX + 1, sheenCenter - sheenHalf);
			float sheenRight = std::min(fillMaxX - 1, sheenCenter + sheenHalf);

			if (sheenRight > sheenLeft)
			{
				float sheenMid = (sheenLeft + sheenRight) * 0.5f;

				float sheenAlpha = isAnimating ? 0.25f : 0.18f;

				drawList->AddRectFilledMultiColor(
					ImVec2(sheenLeft, minY),
					ImVec2(sheenMid, maxY),
					IM_COL32(255, 255, 255, 0),
					IM_COL32(255, 255, 255, (int)(sheenAlpha * 255)),
					IM_COL32(255, 255, 255, (int)((sheenAlpha * 0.55f) * 255)),
					IM_COL32(255, 255, 255, 0)
				);

				drawList->AddRectFilledMultiColor(
					ImVec2(sheenMid, minY),
					ImVec2(sheenRight, maxY),
					IM_COL32(255, 255, 255, (int)(sheenAlpha * 255)),
					IM_COL32(255, 255, 255, 0),
					IM_COL32(255, 255, 255, 0),
					IM_COL32(255, 255, 255, (int)((sheenAlpha * 0.55f) * 255))
				);
			}
		}
	}

	for (int i = 1; i <= 9; ++i)
	{
		float tx = minX + (barW * (i / 10.0f));
		bool reached = tx <= (minX + fillWidth);

		float a = reached ? 0.64f : 0.34f;

		drawList->AddLine(
			ImVec2(tx, minY + 1),
			ImVec2(tx, maxY - 1),
			IM_COL32(255, 255, 255, (int)(a * 255)),
			1.0f
		);
	}

	drawList->AddRect(
		ImVec2(minX, minY),
		ImVec2(maxX, maxY),
		colHighlight,
		Settings.BarRounding,
		0,
		Settings.BarBorderThickness
	);

	std::string text = label.empty()
		? std::to_string((int)std::floor(pct + 0.5f)) + "%"
		: label;

	ImVec2 textSize = ImGui::CalcTextSize(text.c_str());

	float textX = minX + ((maxX - minX - textSize.x) * 0.5f);
	float textY = minY + ((barH - ImGui::GetTextLineHeight()) * 0.5f);

	drawList->AddText(ImVec2(textX + 1, textY + 1), IM_COL32(0, 0, 0, 230), text.c_str());
	drawList->AddText(ImVec2(textX, textY), IM_COL32(255, 255, 255, 255), text.c_str());
}

void Ui::RenderFancyHPBar(
	const std::string& id,
	float hpPct,
	float height,
	float width,
	ImU32 hpHighlight,
	const std::string& label)
{
	ImVec4 hpLow = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
	ImVec4 hpMid = ImVec4(0.9f, 0.7f, 0.2f, 1.0f);
	ImVec4 hpHigh = ImVec4(0.2f, 0.9f, 0.2f, 1.0f);

	RenderAnimatedPercentage(
		id,
		hpPct,
		height,
		width,
		hpLow,
		hpMid,
		hpHigh,
		hpHighlight,
		label
	);
}
