#pragma once

#include "ConfigVariable.h"
#include "Widgets.h"

#include <string>

namespace Ui {

class Config
{
    Config();

public:
    static Config& Get();

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    void SaveSettings();
    void LoadSettings();

private:
    std::string m_configFile;

    // must come before the variables so that it's initialized when they register themselves in their constructors
    ConfigContainer m_container;

public:
    // Rendering toggles
    ConfigVariable<bool> RenderForSelf{ m_container, "RenderForSelf", "Render For Self", true 
    };
    ConfigVariable<bool> RenderForGroup{ m_container, "RenderForGroup", "Render For Group" , true };
    ConfigVariable<bool> RenderForTarget{ m_container, "RenderForTarget", "Render For Target", true };
    ConfigVariable<bool> RenderForAllHaters{ m_container, "RenderForAllHaters", "Render For Haters", true };

    // Optional display
    ConfigVariable<bool> ShowGuild{ m_container, "ShowGuild", "Show Guild", false };
    ConfigVariable<bool> ShowPurpose{ m_container, "ShowPurpose", "Show Purpose", false };
    ConfigVariable<bool> ShowLevel{ m_container, "ShowLevel", "Show Level", true };
    ConfigVariable<bool> ShowClass{ m_container, "ShowClass", "Show Class", true };
    ConfigVariable<bool> ShortClassName{ m_container, "ShortClassName", "Short Class Name", true };
    ConfigVariable<bool> ShowTargetIndicatorWings{ m_container, "ShowTargetIndicatorWings", "Show Target Indicator", true };
    ConfigVariable<float> TargetIndicatorWingLength{ m_container, "TargetIndicatorWingLength", "Target Indicator Length", 15.0f, 5.0f, 75.0f, "%.0f" };
    ConfigVariable<bool> DrawBarBorders{ m_container, "DrawBarBorders", "Draw Bar Borders", true };

    // Rendering behavior
    ConfigVariable<bool> RenderToForeground{ m_container, "RenderToForeground", "Render To Foreground", false };
    ConfigVariable<bool> RenderNoLOS{ m_container, "RenderNoLOS", "Render Even When Occluded", false };

    // Basic flags
    ConfigVariable<bool> ShowBuffIcons{ m_container, "ShowBuffIcons", "Show Buff Icons", true };
    ConfigVariable<bool> ShowDebugPanel{ m_container, "ShowDebugPanel", "Show Debug Panel", false };

    ConfigVariable<bool> DrawTestBar{ m_container, "DrawTestBar", "Draw Test Bar", false };
    ConfigVariable<float> BarPercent{ m_container, "BarPercent", "Test Bar Percent", 100.0f, 0.0f, 100.0f, "%0.f" };

    // Layout / sizes
    ConfigVariable<float> FontSize{ m_container, "FontSize", "Font Size", 20.0f, 1.0f, 40.0f, "%.1f" };
    ConfigVariable<float> IconSize{ m_container, "IconSize", "Icon Size", 20.0f, 10.0f, 40.0f, "%.1f" };
    ConfigVariable<float> NameplateWidth{ m_container, "NameplateWidth", "Nameplate Width", 500.0f, 25.0f, 1500.0f, "%0.f" };
    ConfigVariable<int> HPTicks{ m_container, "HPTicks", "HP Ticks Every [x]%", 10, 1, 25, "%d" };
    ConfigVariable<float> NameplateHeightOffset{ m_container, "NameplateHeightOffset", "Nameplate Height Offset", 35.0f, -100.0f, 400.0f, "%.0f" };

    // Bar appearance
    ConfigVariable<float> BarRounding{ m_container, "BarRounding", "Bar Roudning", 6.0f, 0.0f, 10.0f, "%.1f" };
    ConfigVariable<float> BarBorderThickness{ m_container, "BarBorderThickness", "Bar Border Thickness", 2.5f, 0.0f, 5.0f, "%.1f" };

    // HP bar style
    ConfigVariable<int> HPBarStyleSelf{ m_container, "HPBarStyleSelf", "Self HP Bar Style", static_cast<int>(HPBarStyle_ColorRange),
        [](ConfigVariable<int>& var, float) {
            int v = var.get();
            if (Ui::AnimatedCombo(var.getDisplayName().c_str(), &v, { "Solid Red", "Con Color", "Color Range" }))
            { var.set(v); Config::Get().SaveSettings(); }
        }
    };
    ConfigVariable<int> HPBarStyleGroup{ m_container, "HPBarStyleGroup", "Group HP Bar Style", static_cast<int>(HPBarStyle_ColorRange),
        [](ConfigVariable<int>& var, float) {
            int v = var.get();
            if (Ui::AnimatedCombo(var.getDisplayName().c_str(), &v, { "Solid Red", "Con Color", "Color Range" }))
            { var.set(v); Config::Get().SaveSettings(); }
        }
    };
    ConfigVariable<int> HPBarStyleTarget{ m_container, "HPBarStyleTarget", "Target HP Bar Style", static_cast<int>(HPBarStyle_ColorRange),
        [](ConfigVariable<int>& var, float) {
            int v = var.get();
            if (Ui::AnimatedCombo(var.getDisplayName().c_str(), &v, { "Solid Red", "Con Color", "Color Range" }))
            { var.set(v); Config::Get().SaveSettings(); }
        }
    };
    ConfigVariable<int> HPBarStyleHaters{ m_container, "HPBarStyleHaters", "Haters HP Bar Style", static_cast<int>(HPBarStyle_ColorRange),
        [](ConfigVariable<int>& var, float) {
            int v = var.get();
            if (Ui::AnimatedCombo(var.getDisplayName().c_str(), &v, { "Solid Red", "Con Color", "Color Range" }))
            { var.set(v); Config::Get().SaveSettings(); }
        }
    };
};

} // namespace Ui
